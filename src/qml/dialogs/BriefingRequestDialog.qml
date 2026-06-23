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
import Qt.labs.settings

import akaflieg_freiburg.enroute
import "../items"

CenteringDialog {
    id: dlg

    title: qsTr("Analyze Route")
    modal: true
    standardButtons: DialogButtonBox.Close

    // Persisted form fields
    Settings {
        id: settings
        category: "BriefingRequest"
        property string alternate:   ""
        property string temsiToken:  ""
        property int    providerIdx: 0
        property string usableFuelL: ""
    }

    // Switch to result view when report arrives
    Connections {
        target: BriefingProvider
        function onResultChanged() {
            stackView.push(resultPage)
        }
        function onStatusChanged() {
            if (BriefingProvider.status === BriefingProvider.Error)
                errorLabel.visible = true
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: formPage
    }

    // ── Form page ────────────────────────────────────────────────────────────
    Component {
        id: formPage

        Item {
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 8

                // Usable fuel
                Label { text: qsTr("Usable fuel at departure (L)") }
                MyTextField {
                    id: usableFuelField
                    Layout.fillWidth: true
                    placeholderText: "120"
                    text: settings.usableFuelL
                    inputMethodHints: Qt.ImhDigitsOnly
                    validator: DoubleValidator { bottom: 0; top: 9999; decimals: 1 }
                    onTextChanged: settings.usableFuelL = text
                }

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

                // Error
                Label {
                    id: errorLabel
                    Layout.fillWidth: true
                    visible: false
                    color: "red"
                    wrapMode: Text.Wrap
                    text: BriefingProvider.errorMessage
                }

                // Submit row
                RowLayout {
                    Layout.fillWidth: true
                    BusyIndicator {
                        visible: BriefingProvider.status === BriefingProvider.Loading
                        running: visible
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Request Briefing")
                        enabled: BriefingProvider.status !== BriefingProvider.Loading
                        onClicked: {
                            PlatformAdaptor.vibrateBrief()
                            errorLabel.visible = false
                            BriefingProvider.requestBriefing(
                                alternateField.text.toUpperCase(),
                                parseFloat(usableFuelField.text) || 0.0,
                                temsiField.text
                            )
                        }
                    }
                }
            }
        }
    }

    // ── Result page ──────────────────────────────────────────────────────────
    Component {
        id: resultPage

        Item {
            // Back button at the top
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 0

                Button {
                    text: qsTr("← New Request")
                    flat: true
                    onClicked: {
                        PlatformAdaptor.vibrateBrief()
                        stackView.pop()
                    }
                }

                // Margins quick-glance strip
                Rectangle {
                    Layout.fillWidth: true
                    height: marginsRow.implicitHeight + 12
                    color: {
                        if (isNaN(BriefingProvider.marginL)) return "transparent"
                        return BriefingProvider.marginL >= 0 ? "#1a7f1a" : "#c0392b"
                    }
                    radius: 4

                    Row {
                        id: marginsRow
                        anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
                        anchors.margins: 8
                        spacing: 16

                        // Fuel margin
                        Column {
                            spacing: 2
                            Label {
                                text: qsTr("Fuel margin")
                                font.pixelSize: 10
                                color: "white"
                                opacity: 0.8
                            }
                            Label {
                                text: {
                                    if (isNaN(BriefingProvider.marginL)) return "–"
                                    const m = BriefingProvider.marginL
                                    const sign = m >= 0 ? "+" : ""
                                    return sign + m.toFixed(0) + " L"
                                }
                                font.bold: true
                                color: "white"
                            }
                        }

                        // Endurance at destination
                        Column {
                            spacing: 2
                            Label {
                                text: qsTr("Endurance@dest")
                                font.pixelSize: 10
                                color: "white"
                                opacity: 0.8
                            }
                            Label {
                                text: {
                                    if (isNaN(BriefingProvider.enduranceAtDestMin)) return "–"
                                    return BriefingProvider.enduranceAtDestMin.toFixed(0) + " min"
                                }
                                font.bold: true
                                color: "white"
                            }
                        }

                        // ETA vs sunset
                        Column {
                            spacing: 2
                            visible: !isNaN(BriefingProvider.etaVsSunsetMin)
                            Label {
                                text: qsTr("Before sunset")
                                font.pixelSize: 10
                                color: "white"
                                opacity: 0.8
                            }
                            Label {
                                text: {
                                    if (isNaN(BriefingProvider.etaVsSunsetMin)) return "–"
                                    return BriefingProvider.etaVsSunsetMin.toFixed(0) + " min"
                                }
                                font.bold: true
                                color: "white"
                            }
                        }
                    }
                }

                // Scrollable content: markdown report + TEMSI chart
                DecoratedScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: parent.width
                        spacing: 12

                        // Markdown report
                        Label {
                            Layout.fillWidth: true
                            text: BriefingProvider.report
                            textFormat: Text.MarkdownText
                            wrapMode: Text.Wrap
                            onLinkActivated: Qt.openUrlExternally(link)
                        }

                        // TEMSI chart (only when chart_data is present)
                        Image {
                            Layout.fillWidth: true
                            visible: BriefingProvider.chartData !== ""
                            source: BriefingProvider.chartData !== ""
                                    ? "data:image/png;base64," + BriefingProvider.chartData
                                    : ""
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                        }
                    }
                }
            }
        }
    }

    // Reset to form when dialog is opened
    onOpened: {
        if (stackView.depth > 1)
            stackView.pop(null, StackView.Immediate)
    }
}
