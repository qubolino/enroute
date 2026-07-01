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
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrlQuery>
#include <cmath>

using namespace Qt::Literals::StringLiterals;

Weather::BriefingProvider::BriefingProvider(QObject* parent)
    : QObject(parent)
{
    QSettings s;
    m_serverUrl   = s.value(u"BriefingProvider/serverUrl"_s).toString();
    m_llmProvider = s.value(u"BriefingProvider/llmProvider"_s, u"anthropic"_s).toString();

    // Restore last result
    m_briefingId         = s.value(u"BriefingProvider/briefingId"_s).toString();
    m_report             = s.value(u"BriefingProvider/report"_s).toString();
    m_fuelAtDestinationL = s.value(u"BriefingProvider/fuelAtDestinationL"_s, qQNaN()).toDouble();
    m_legalReserveL      = s.value(u"BriefingProvider/legalReserveL"_s,      qQNaN()).toDouble();
    m_marginL            = s.value(u"BriefingProvider/marginL"_s,            qQNaN()).toDouble();
    m_enduranceAtDestMin = s.value(u"BriefingProvider/enduranceAtDestMin"_s, qQNaN()).toDouble();
    m_etaVsSunsetMin     = s.value(u"BriefingProvider/etaVsSunsetMin"_s,     qQNaN()).toDouble();

    const QString chartBase64 = s.value(u"BriefingProvider/chartBase64"_s).toString();
    if (!chartBase64.isEmpty()) {
        const QByteArray pngData = QByteArray::fromBase64(chartBase64.toLatin1());
        m_chartTempFile.setFileTemplate(QDir::tempPath() + u"/enroute_chart_XXXXXX.png"_s);
        m_chartTempFile.setAutoRemove(false);
        if (m_chartTempFile.open()) {
            m_chartTempFile.write(pngData);
            m_chartTempFile.flush();
            m_chartUrl = QUrl::fromLocalFile(m_chartTempFile.fileName());
        }
    }
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
    if (m_status != s)
        m_status = s;
    emit statusChanged();
}

void Weather::BriefingProvider::clearResult()
{
    m_briefingId.clear();
    m_report.clear();
    m_chartUrl.clear();
    if (m_chartTempFile.isOpen()) m_chartTempFile.close();
    m_chartTempFile.remove();
    m_fuelAtDestinationL = qQNaN();
    m_legalReserveL      = qQNaN();
    m_marginL            = qQNaN();
    m_enduranceAtDestMin = qQNaN();
    m_etaVsSunsetMin     = qQNaN();
}

QString Weather::BriefingProvider::parseResponse(const QJsonObject& root)
{
    const QString report = root[u"report"_s].toString();
    if (report.isEmpty())
        return tr("Server returned an empty report.");

    m_report     = report;
    m_briefingId = root[u"id"_s].toString();

    // Decode chart PNG → temp file so QML Image can load it
    const QString chartBase64 = root[u"chart_data"_s].toString();
    m_chartUrl.clear();
    m_chartTempFile.remove();
    if (!chartBase64.isEmpty()) {
        const QByteArray pngData = QByteArray::fromBase64(chartBase64.toLatin1());
        m_chartTempFile.setFileTemplate(QDir::tempPath() + u"/enroute_chart_XXXXXX.png"_s);
        m_chartTempFile.setAutoRemove(false);
        if (m_chartTempFile.open()) {
            m_chartTempFile.write(pngData);
            m_chartTempFile.flush();
            m_chartUrl = QUrl::fromLocalFile(m_chartTempFile.fileName());
        }
    }

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

    // Persist so the result survives tab switches and app restarts
    QSettings ps;
    ps.setValue(u"BriefingProvider/briefingId"_s,         m_briefingId);
    ps.setValue(u"BriefingProvider/report"_s,             m_report);
    ps.setValue(u"BriefingProvider/fuelAtDestinationL"_s, m_fuelAtDestinationL);
    ps.setValue(u"BriefingProvider/legalReserveL"_s,      m_legalReserveL);
    ps.setValue(u"BriefingProvider/marginL"_s,            m_marginL);
    ps.setValue(u"BriefingProvider/enduranceAtDestMin"_s, m_enduranceAtDestMin);
    ps.setValue(u"BriefingProvider/etaVsSunsetMin"_s,     m_etaVsSunsetMin);
    ps.setValue(u"BriefingProvider/chartBase64"_s,        chartBase64);

    return {};
}

void Weather::BriefingProvider::requestBriefing(double usableFuelL,
                                                 const QString& plannedOffBlock)
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

    QJsonArray waypointsArray;
    for (const auto& wp : wps) {
        const QGeoCoordinate coord = wp.coordinate();
        if (!coord.isValid()) continue;
        QJsonObject wpObj;
        const QString icao = wp.ICAOCode();
        wpObj[u"name"_s] = icao.isEmpty() ? wp.name() : icao;
        wpObj[u"lat"_s]  = coord.latitude();
        wpObj[u"lon"_s]  = coord.longitude();
        if (coord.type() == QGeoCoordinate::Coordinate3D && coord.altitude() > -1000.0)
            wpObj[u"alt_m"_s] = coord.altitude();
        waypointsArray.append(wpObj);
    }
    if (waypointsArray.size() < 2) {
        setStatus(Status::Error, tr("Route must contain at least two valid waypoints."));
        return;
    }

    const auto aircraft = nav->aircraft();
    QJsonObject aircraftObj;
    aircraftObj[u"tas_kt"_s]             = aircraft.cruiseSpeed().isFinite()
                                               ? aircraft.cruiseSpeed().toKN() : 0.0;
    aircraftObj[u"fuel_flow_lph"_s]      = aircraft.fuelConsumption().isFinite()
                                               ? aircraft.fuelConsumption().toLPH() : 0.0;
    aircraftObj[u"usable_fuel_liters"_s] = usableFuelL;
    if (!aircraft.name().isEmpty())
        aircraftObj[u"registration"_s] = aircraft.name();
    const double cruiseAltFt = nav->cruiseAltitudeFt();
    if (!std::isnan(cruiseAltFt) && cruiseAltFt > 0.0)
        aircraftObj[u"cruise_altitude_ft"_s] = cruiseAltFt;

    QJsonObject body;
    body[u"waypoints"_s]         = waypointsArray;
    body[u"planned_off_block"_s] = plannedOffBlock;
    body[u"aircraft"_s]          = aircraftObj;
    if (!m_llmProvider.isEmpty())
        body[u"provider"_s]      = m_llmProvider;

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
        const QJsonObject root = QJsonDocument::fromJson(reply->readAll()).object();
        const QString err = parseResponse(root);
        if (!err.isEmpty()) {
            setStatus(Status::Error, err);
            return;
        }
        setStatus(Status::Idle);
        emit resultChanged();
    });
}

void Weather::BriefingProvider::loadBriefing(const QString& idOrUrl)
{
    if (m_status == Status::Loading) return;

    if (m_serverUrl.isEmpty()) {
        setStatus(Status::Error, tr("Briefing server URL is not configured."));
        return;
    }

    // Extract the 12-char hex ID from either a bare ID or a breef.ing URL
    QString id = idOrUrl.trimmed();
    if (id.contains(u"?id="_s)) {
        QUrl url(id);
        id = QUrlQuery(url.query()).queryItemValue(u"id"_s);
    }
    if (id.isEmpty()) {
        setStatus(Status::Error, tr("Could not extract a briefing ID from the given URL."));
        return;
    }

    clearResult();
    setStatus(Status::Loading);

    QNetworkRequest request(QUrl(m_serverUrl + u"/briefing/"_s + id));
    auto* reply = m_nam.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            setStatus(Status::Error, reply->errorString());
            return;
        }
        const QJsonObject root = QJsonDocument::fromJson(reply->readAll()).object();
        const QString err = parseResponse(root);
        if (!err.isEmpty()) {
            setStatus(Status::Error, err);
            return;
        }
        setStatus(Status::Idle);
        emit resultChanged();
    });
}
