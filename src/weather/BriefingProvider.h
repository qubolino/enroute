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

namespace Weather {

/*! \brief Posts a flight plan + aircraft profile to the preflight briefing server
 *  and exposes the returned markdown report to QML.
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

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString report READ report NOTIFY reportChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY statusChanged)
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QString llmProvider READ llmProvider WRITE setLlmProvider NOTIFY llmProviderChanged)

    //
    // Getters
    //

    [[nodiscard]] Status status() const { return m_status; }
    [[nodiscard]] QString report() const { return m_report; }
    [[nodiscard]] QString errorMessage() const { return m_errorMessage; }
    [[nodiscard]] QString serverUrl() const { return m_serverUrl; }
    [[nodiscard]] QString llmProvider() const { return m_llmProvider; }

    //
    // Setters
    //

    void setServerUrl(const QString& url);
    void setLlmProvider(const QString& provider);

    /*! \brief POST a briefing request to the server.
     *
     *  Reads the current route and aircraft from Navigator. Sets status to
     *  Loading immediately; emits reportChanged on success or statusChanged
     *  (with Error) on failure.
     *
     *  @param alternate  ICAO code of alternate aerodrome, or empty string.
     *  @param temsiToken Aeroweb login= token, or empty string.
     */
    Q_INVOKABLE void requestBriefing(const QString& alternate, const QString& temsiToken);

signals:
    void statusChanged();
    void reportChanged();
    void serverUrlChanged();
    void llmProviderChanged();

private:
    void setStatus(Status s, const QString& error = {});

    QNetworkAccessManager m_nam;
    Status  m_status       {Status::Idle};
    QString m_report;
    QString m_errorMessage;
    QString m_serverUrl;
    QString m_llmProvider  {QStringLiteral("anthropic")};
};

} // namespace Weather
