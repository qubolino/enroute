/***************************************************************************
 *   Copyright (C) 2026 by Quentin Bossard                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "weather/BriefingProvider.h"
#include "GlobalObject.h"
#include "navigation/Navigator.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <cmath>

using namespace Qt::Literals::StringLiterals;

Weather::BriefingProvider::BriefingProvider(QObject* parent)
    : QObject(parent)
{
    QSettings s;
    m_serverUrl   = s.value(u"BriefingProvider/serverUrl"_s).toString();
    m_llmProvider = s.value(u"BriefingProvider/llmProvider"_s, u"anthropic"_s).toString();
}

Weather::BriefingProvider* Weather::BriefingProvider::create(QQmlEngine*, QJSEngine*)
{
    static BriefingProvider instance;
    return &instance;
}

Weather::BriefingProvider* Weather::BriefingProvider::instance()
{
    return create(nullptr, nullptr);
}

void Weather::BriefingProvider::setServerUrl(const QString& url)
{
    if (m_serverUrl == url) return;
    m_serverUrl = url;
    QSettings().setValue(u"BriefingProvider/serverUrl"_s, url);
    emit serverUrlChanged();
}

void Weather::BriefingProvider::setLlmProvider(const QString& provider)
{
    if (m_llmProvider == provider) return;
    m_llmProvider = provider;
    QSettings().setValue(u"BriefingProvider/llmProvider"_s, provider);
    emit llmProviderChanged();
}

void Weather::BriefingProvider::setStatus(Status s, const QString& error)
{
    m_errorMessage = error;
    if (m_status != s) {
        m_status = s;
    }
    emit statusChanged();
}

void Weather::BriefingProvider::clearResult()
{
    m_report.clear();
    m_chartData.clear();
    m_fuelAtDestinationL = qQNaN();
    m_legalReserveL      = qQNaN();
    m_marginL            = qQNaN();
    m_enduranceAtDestMin = qQNaN();
    m_etaVsSunsetMin     = qQNaN();
}

void Weather::BriefingProvider::requestBriefing(const QString& alternate,
                                                 double usableFuelL,
                                                 const QString& temsiToken)
{
    if (m_status == Status::Loading) return;

    if (m_serverUrl.isEmpty()) {
        setStatus(Status::Error, tr("Briefing server URL is not configured."));
        return;
    }

    if (usableFuelL <= 0.0 || std::isnan(usableFuelL)) {
        setStatus(Status::Error, tr("Usable fuel at departure must be greater than zero."));
        return;
    }

    auto* nav       = GlobalObject::navigator();
    const auto& wps = nav->flightRoute()->waypoints();

    if (wps.size() < 2) {
        setStatus(Status::Error, tr("Route must contain at least two waypoints."));
        return;
    }

    // Build waypoints[] array — preferred format (carries coords + altitude for every point).
    // The server falls back to ICAO lookup only when lat/lon are absent, so we always include
    // coordinates. This ensures non-ICAO points (VFR reporting points, mountain passes) are
    // handled correctly by the server's wind interpolation and METAR station selection.
    QJsonArray waypointsArray;
    for (const auto& wp : wps) {
        const QGeoCoordinate coord = wp.coordinate();
        if (!coord.isValid()) continue;

        QJsonObject wpObj;
        // Use ICAO code as name when available, otherwise the waypoint name
        const QString icao = wp.ICAOCode();
        wpObj[u"name"_s] = icao.isEmpty() ? wp.name() : icao;
        wpObj[u"lat"_s]  = coord.latitude();
        wpObj[u"lon"_s]  = coord.longitude();
        // Include elevation when the coordinate is 3D (waypoints that have known field elevation)
        if (coord.type() == QGeoCoordinate::Coordinate3D && coord.altitude() > -1000.0) {
            wpObj[u"alt_m"_s] = coord.altitude();
        }
        waypointsArray.append(wpObj);
    }
    if (waypointsArray.size() < 2) {
        setStatus(Status::Error, tr("Route must contain at least two valid waypoints."));
        return;
    }

    // Aircraft sub-object
    const auto aircraft = nav->aircraft();
    QJsonObject aircraftObj;
    aircraftObj[u"tas_kt"_s]             = aircraft.cruiseSpeed().isFinite()
                                               ? aircraft.cruiseSpeed().toKN() : 0.0;
    aircraftObj[u"fuel_flow_lph"_s]      = aircraft.fuelConsumption().isFinite()
                                               ? aircraft.fuelConsumption().toLPH() : 0.0;
    aircraftObj[u"usable_fuel_liters"_s] = usableFuelL;
    if (!aircraft.name().isEmpty())
        aircraftObj[u"registration"_s] = aircraft.name();
    // Cruise altitude as fallback for points that lack alt_m
    const double cruiseAltFt = nav->cruiseAltitudeFt();
    if (!std::isnan(cruiseAltFt) && cruiseAltFt > 0.0)
        aircraftObj[u"cruise_altitude_ft"_s] = cruiseAltFt;

    // Departure time
    QDateTime dep = nav->departureTime();
    if (!dep.isValid()) dep = QDateTime::currentDateTimeUtc();
    const QString depStr = dep.toUTC().toString(Qt::ISODateWithMs).replace(u"+00:00"_s, u"Z"_s);

    // Request body
    QJsonObject body;
    body[u"waypoints"_s]           = waypointsArray;
    body[u"planned_off_block"_s]   = depStr;
    body[u"aircraft"_s]            = aircraftObj;
    if (!alternate.trimmed().isEmpty())
        body[u"alternate"_s]       = alternate.trimmed().toUpper();
    if (!m_llmProvider.isEmpty())
        body[u"provider"_s]        = m_llmProvider;
    if (!temsiToken.trimmed().isEmpty())
        body[u"temsi_login_token"_s] = temsiToken.trimmed();

    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);

    QNetworkRequest request(QUrl(m_serverUrl + u"/briefing"_s));
    request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/json"_s);

    clearResult();
    setStatus(Status::Loading);

    auto* reply = m_nam.post(request, payload);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            setStatus(Status::Error, reply->errorString());
            return;
        }

        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const QJsonObject root = doc.object();

        const QString report = root[u"report"_s].toString();
        if (report.isEmpty()) {
            setStatus(Status::Error, tr("Server returned an empty report."));
            return;
        }
        m_report    = report;
        m_chartData = root[u"chart_data"_s].toString(); // "" when null/absent

        // Parse margins
        const QJsonObject margins = root[u"data"_s][u"margins"_s].toObject();
        auto getDouble = [&](const QString& key) -> double {
            const QJsonValue v = margins[key];
            return v.isDouble() ? v.toDouble() : qQNaN();
        };
        m_fuelAtDestinationL = getDouble(u"fuel_at_destination_liters"_s);
        m_legalReserveL      = getDouble(u"legal_reserve_liters"_s);
        m_marginL            = getDouble(u"margin_liters"_s);
        m_enduranceAtDestMin = getDouble(u"endurance_at_destination_min"_s);
        m_etaVsSunsetMin     = getDouble(u"eta_vs_sunset_min"_s);

        setStatus(Status::Idle);
        emit resultChanged();
    });
}
