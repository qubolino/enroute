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

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QQmlEngine>
#include <QTemporaryFile>
#include <QUrl>

namespace Weather {

/*! \brief Posts a flight plan + aircraft profile to the preflight briefing server
 *  and exposes the returned markdown report, margins summary, and TEMSI chart to QML.
 *  Also supports loading a previously stored briefing by ID or breef.ing URL.
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
    Q_PROPERTY(QString briefingId READ briefingId NOTIFY resultChanged)  // 12-char hex, "" if none
    Q_PROPERTY(QString report     READ report     NOTIFY resultChanged)
    Q_PROPERTY(QUrl    chartUrl   READ chartUrl   NOTIFY resultChanged)  // file:// URL, empty if none

    // Margins (from data.margins) — NaN when not available
    Q_PROPERTY(double fuelAtDestinationL READ fuelAtDestinationL NOTIFY resultChanged)
    Q_PROPERTY(double legalReserveL      READ legalReserveL      NOTIFY resultChanged)
    Q_PROPERTY(double marginL            READ marginL            NOTIFY resultChanged)
    Q_PROPERTY(double enduranceAtDestMin READ enduranceAtDestMin NOTIFY resultChanged)
    Q_PROPERTY(double etaVsSunsetMin     READ etaVsSunsetMin     NOTIFY resultChanged)

    //
    // Getters
    //

    [[nodiscard]] Status  status()       const { return m_status; }
    [[nodiscard]] QString errorMessage() const { return m_errorMessage; }
    [[nodiscard]] QString serverUrl()    const { return m_serverUrl; }
    [[nodiscard]] QString llmProvider()  const { return m_llmProvider; }
    [[nodiscard]] QString briefingId()   const { return m_briefingId; }
    [[nodiscard]] QString report()       const { return m_report; }
    [[nodiscard]] QUrl    chartUrl()     const { return m_chartUrl; }
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

    /*! \brief POST a new briefing request to the server.
     *  @param usableFuelL      Total usable fuel at departure in litres.
     *  @param plannedOffBlock  ISO 8601 UTC string, e.g. "2026-06-24T10:30:00Z".
     */
    Q_INVOKABLE void requestBriefing(double usableFuelL, const QString& plannedOffBlock);

    /*! \brief Load a previously stored briefing by ID or URL.
     *  Accepts a bare 12-char hex ID ("9b2a5796e1ca") or a full
     *  breef.ing URL ("https://breef.ing/?id=9b2a5796e1ca").
     *  GETs from m_serverUrl/briefing/{id}.
     */
    Q_INVOKABLE void loadBriefing(const QString& idOrUrl);

signals:
    void statusChanged();
    void resultChanged();
    void serverUrlChanged();
    void llmProviderChanged();

private:
    void setStatus(Status s, const QString& error = {});
    void clearResult();
    /*! Parses a BriefingResponse JSON object into member fields and persists to QSettings.
     *  Returns an error string on failure, empty string on success. */
    QString parseResponse(const QJsonObject& root);

    QNetworkAccessManager m_nam;
    Status  m_status        {Status::Idle};
    QString m_errorMessage;
    QString m_serverUrl;
    QString m_llmProvider   {QStringLiteral("anthropic")};

    // Result fields
    QString        m_briefingId;
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
