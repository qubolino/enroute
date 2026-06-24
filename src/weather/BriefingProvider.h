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

#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QQmlEngine>
#include <QTemporaryFile>
#include <QUrl>

namespace Weather {

/*! \brief Posts a flight plan + aircraft profile to the preflight briefing server
 *  and exposes the returned markdown report, margins summary, and TEMSI chart to QML.
 */
class BriefingProvider : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum class Status { Idle, Loading, Error };
    Q_ENUM(Status)

    explicit BriefingProvider(QObject* parent = nullptr);
    static Weather::BriefingProvider* create(QQmlEngine*, QJSEngine*);
    static Weather::BriefingProvider* instance();

    //
    // Properties
    //

    Q_PROPERTY(Status  status       READ status       NOTIFY statusChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY statusChanged)

    // Settings
    Q_PROPERTY(QString serverUrl   READ serverUrl   WRITE setServerUrl   NOTIFY serverUrlChanged)
    Q_PROPERTY(QString llmProvider READ llmProvider WRITE setLlmProvider NOTIFY llmProviderChanged)

    // Result — all populated together when a request succeeds
    Q_PROPERTY(QString report    READ report    NOTIFY resultChanged)
    Q_PROPERTY(QUrl    chartUrl  READ chartUrl  NOTIFY resultChanged)  // file:// URL, empty if none

    // Margins (from data.margins) — NaN when not available
    Q_PROPERTY(double fuelAtDestinationL    READ fuelAtDestinationL    NOTIFY resultChanged)
    Q_PROPERTY(double legalReserveL         READ legalReserveL         NOTIFY resultChanged)
    Q_PROPERTY(double marginL               READ marginL               NOTIFY resultChanged)
    Q_PROPERTY(double enduranceAtDestMin    READ enduranceAtDestMin    NOTIFY resultChanged)
    Q_PROPERTY(double etaVsSunsetMin        READ etaVsSunsetMin        NOTIFY resultChanged)  // NaN if unavailable

    //
    // Getters
    //

    [[nodiscard]] Status  status()       const { return m_status; }
    [[nodiscard]] QString errorMessage() const { return m_errorMessage; }
    [[nodiscard]] QString serverUrl()    const { return m_serverUrl; }
    [[nodiscard]] QString llmProvider()  const { return m_llmProvider; }
    [[nodiscard]] QString report()    const { return m_report; }
    [[nodiscard]] QUrl    chartUrl()  const { return m_chartUrl; }
    [[nodiscard]] double  fuelAtDestinationL()  const { return m_fuelAtDestinationL; }
    [[nodiscard]] double  legalReserveL()       const { return m_legalReserveL; }
    [[nodiscard]] double  marginL()             const { return m_marginL; }
    [[nodiscard]] double  enduranceAtDestMin()  const { return m_enduranceAtDestMin; }
    [[nodiscard]] double  etaVsSunsetMin()      const { return m_etaVsSunsetMin; }

    //
    // Setters / invokables
    //

    void setServerUrl(const QString& url);
    void setLlmProvider(const QString& provider);

    /*! \brief POST a briefing request to the server.
     *
     *  Reads the current route from Navigator and builds a `waypoints[]` request
     *  (name + lat/lon/alt_m per waypoint). Falls back to the legacy `route[]` array
     *  only when every waypoint is an ICAO airport.
     *
     *  @param alternate    ICAO code of alternate aerodrome, or empty string.
     *  @param usableFuelL  Usable fuel at departure in litres.
     *  @param temsiToken   Aeroweb login= token, or empty string.
     */
    /*! @param usableFuelL      Total usable fuel at departure in litres.
     *  @param plannedOffBlock  ISO 8601 UTC string, e.g. "2026-06-24T10:30:00Z".
     */
    Q_INVOKABLE void requestBriefing(double usableFuelL, const QString& plannedOffBlock);

signals:
    void statusChanged();
    void resultChanged();
    void serverUrlChanged();
    void llmProviderChanged();

private:
    void setStatus(Status s, const QString& error = {});
    void clearResult();

    QNetworkAccessManager m_nam;
    Status  m_status        {Status::Idle};
    QString m_errorMessage;
    QString m_serverUrl;
    QString m_llmProvider   {QStringLiteral("anthropic")};

    // Result fields
    QString        m_report;
    QUrl           m_chartUrl;
    QTemporaryFile m_chartTempFile;
    double  m_fuelAtDestinationL {qQNaN()};
    double  m_legalReserveL      {qQNaN()};
    double  m_marginL            {qQNaN()};
    double  m_enduranceAtDestMin {qQNaN()};
    double  m_etaVsSunsetMin     {qQNaN()};
};

} // namespace Weather
