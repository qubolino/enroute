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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Settings

import akaflieg_freiburg.enroute
import "../items"

CenteringDialog {
    id: dlg

    title: qsTr("Analyze Route")
    modal: true
    standardButtons: DialogButtonBox.Cancel

    // Persisted fields
    Settings {
        id: settings
        category: "BriefingRequest"
        property string alternate:   ""
        property string temsiToken:  ""
        property int    providerIdx: 0
    }

    // Open the result dialog when report arrives
    Connections {
        target: BriefingProvider
        function onReportChanged() {
            dlg.close()
            briefingResultDialog.open()
        }
        function onStatusChanged() {
            if (BriefingProvider.status === BriefingProvider.Error)
                errorLabel.visible = true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        // Alternate
        Label { text: qsTr("Alternate (optional ICAO)") }
        MyTextField {
            id: alternateField
            Layout.fillWidth: true
            placeholderText: "LFBD"
            text: settings.alternate
            inputMethodHints: Qt.ImhUppercaseOnly
            maximumLength: 4
            onTextChanged: settings.alternate = text.toUpperCase()
        }

        // LLM Provider
        Label { text: qsTr("LLM provider") }
        ComboBox {
            id: providerCombo
            Layout.fillWidth: true
            model: ["anthropic", "mistral", "ollama"]
            currentIndex: settings.providerIdx
            onCurrentIndexChanged: {
                settings.providerIdx = currentIndex
                BriefingProvider.llmProvider = currentValue
            }
            Component.onCompleted: BriefingProvider.llmProvider = currentValue
        }

        // TEMSI token
        Label { text: qsTr("TEMSI token (optional)") }
        MyTextField {
            id: temsiField
            Layout.fillWidth: true
            placeholderText: qsTr("Aeroweb login= token")
            text: settings.temsiToken
            onTextChanged: settings.temsiToken = text
        }

        // Error label
        Label {
            id: errorLabel
            Layout.fillWidth: true
            visible: false
            color: "red"
            wrapMode: Text.Wrap
            text: BriefingProvider.errorMessage
        }

        // Loading / request button
        RowLayout {
            Layout.fillWidth: true

            BusyIndicator {
                visible: BriefingProvider.status === BriefingProvider.Loading
                running: visible
            }

            Button {
                id: requestButton
                Layout.fillWidth: true
                text: qsTr("Request Briefing")
                enabled: BriefingProvider.status !== BriefingProvider.Loading
                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    errorLabel.visible = false
                    BriefingProvider.requestBriefing(
                        alternateField.text.toUpperCase(),
                        temsiField.text
                    )
                }
            }
        }
    }

    onOpened: {
        errorLabel.visible = false
        alternateField.text = settings.alternate
        temsiField.text = settings.temsiToken
        providerCombo.currentIndex = settings.providerIdx
    }

    LongTextDialogMD {
        id: briefingResultDialog
        title: qsTr("Preflight Briefing")
        text: BriefingProvider.report
    }
}
