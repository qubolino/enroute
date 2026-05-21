/***************************************************************************
 *   Copyright (C) 2019-2025 by Stefan Kebekus                             *
 *   stefan.kebekus@gmail.com                                              *
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

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import akaflieg_freiburg.enroute
import "../dialogs"
import "../items"

Page {
    id: aircraftPage
    title: qsTr("Aircraft")

    // Required Properties
    required property var stackView

    // Static objects, used to call static functions
    property speed staticSpeed
    property volumeFlow staticVolumeFlow
    property volume staticVolume

    header: PageHeader {

        height: 60 + SafeInsets.top
        leftPadding: SafeInsets.left
        rightPadding: SafeInsets.right
        topPadding: SafeInsets.top

        ToolButton {
                id: backButton

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                icon.source: "/icons/material/ic_arrow_back.svg"

                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    aircraftPage.stackView.pop()
                }
            }

        Label {
                id: lbl

                anchors.verticalCenter: parent.verticalCenter

                anchors.left: parent.left
                anchors.leftMargin: 72
                anchors.right: headerMenuToolButton.left

                text: aircraftPage.stackView.currentItem.title
                elide: Label.ElideRight
                font.pixelSize: 20
                verticalAlignment: Qt.AlignVCenter
            }

        ToolButton {
                id: headerMenuToolButton

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                icon.source: "/icons/material/ic_more_vert.svg"
                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    headerMenuX.open()
                }

                AutoSizingMenu {
                    id: headerMenuX
                    cascade: true

                    MenuItem {
                        text: qsTr("View Library…")
                        onTriggered: {
                            PlatformAdaptor.vibrateBrief()
                            highlighted = false
                            aircraftPage.stackView.push("AircraftLibrary.qml")
                        }
                    }

                    MenuItem {
                        text: qsTr("Save to library…")
                        onTriggered: {
                            PlatformAdaptor.vibrateBrief()
                            highlighted = false
                            dlgLoader.active = false
                            dlgLoader.source = "../dialogs/AircraftSaveDialog.qml"
                            dlgLoader.active = true
                        }
                    }
                }
        }
    }

    TabBar {
        id: bar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        leftPadding: SafeInsets.left
        rightPadding: SafeInsets.right

        currentIndex: sv.currentIndex

        TabButton { text: qsTr("Common") }
        TabButton { text: qsTr("FPL") }
        TabButton { text: qsTr("W && B") }
    }

    SwipeView {
        id: sv
        anchors.top: bar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: SafeInsets.left
        anchors.rightMargin: SafeInsets.right
        anchors.bottomMargin: SafeInsets.bottom

        clip: true
        currentIndex: bar.currentIndex

    DecoratedScrollView {
        id: acftTab

        contentWidth: width
        clip: true

        // If virtual keyboard come up, make sure that the focused element is visible
        onHeightChanged: {
            if (activeFocusControl != null) {
                contentItem.contentY = activeFocusControl.y - font.pixelSize
            }
        }

        GridLayout {
            id: aircraftTab
            anchors.left: parent.left
            anchors.leftMargin: acftTab.font.pixelSize
            anchors.right: parent.right
            anchors.rightMargin: acftTab.font.pixelSize

            columns: 4

            // Aircraft
            Label {
                text: qsTr("Aircraft")
                Layout.columnSpan: 4
                Layout.topMargin: font.pixelSize/2
                font.pixelSize: acftTab.font.pixelSize*1.2
                font.bold: true
            }

            Label {
                text: qsTr("Name")
                Layout.alignment: Qt.AlignBaseline
                Layout.topMargin: font.pixelSize/2
            }
            MyTextField {
                id: name
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*5
                Layout.topMargin: font.pixelSize/2
                KeyNavigation.tab: transponderCode
                placeholderText: qsTr("Callsign")

                onEditingFinished: Navigator.aircraft.name = text
                text: Navigator.aircraft.name
            }
            ToolButton {
                icon.source: "/icons/material/ic_info_outline.svg"
                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    helpDialog.title = qsTr("Name")
                    helpDialog.text = "<p>"
                            + qsTr("Enter the callsign of your aircraft. Together with the Hex ID, the name is used to filter out your own aircraft when receiving traffic data from the Open Glider Network, preventing it from being displayed twice on the map.")
                            + "</p>"
                    helpDialog.open()
                }
            }

            Label {
                text: qsTr("Hex ID")
                Layout.alignment: Qt.AlignBaseline
                Layout.topMargin: font.pixelSize/2
            }
            MyTextField {
                id: transponderCode
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.topMargin: font.pixelSize/2
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*5
                KeyNavigation.tab: cruiseSpeed
                placeholderText: qsTr("Transponder/FLARM ID")

                onEditingFinished: Navigator.aircraft.transponderCode = text.toUpperCase()
                text: Navigator.aircraft.transponderCode
            }
            ToolButton {
                icon.source: "/icons/material/ic_info_outline.svg"
                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    helpDialog.title = qsTr("Hex ID")
                    helpDialog.text = "<p>"
                            + qsTr("Enter the “ICAO 24-bit address” of your transponder (e.g., “3D1C11”), the “FLARM Radio ID” of your FLARM device (e.g., “3EDE26”) or the “Open Glider Network Source ID” of your aircraft (e.g., “ICA3D1C11”, “FLR123456”). Together with the name, this identifier is used to filter out your own aircraft when receiving traffic data from the Open Glider Network, preventing it from being displayed twice on the map. If you do not use Open Glider Network traffic data, you can leave this field empty.")
                            + "</p><p>"
                            + qsTr("You can enter multiple codes separated by spaces if your aircraft has multiple identifiers. The comparison is case-insensitive.")
                            + "</p>"
                    helpDialog.open()
                }
            }

            Label {
                text: qsTr("ICAO Type")
                Layout.alignment: Qt.AlignBaseline
                Layout.topMargin: font.pixelSize/2
            }
            MyTextField {
                id: icaoType
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*5
                Layout.topMargin: font.pixelSize/2
                placeholderText: qsTr("e.g. C172")

                onEditingFinished: Navigator.aircraft.icaoType = text.toUpperCase()
                text: Navigator.aircraft.icaoType
            }
            ToolButton {
                icon.source: "/icons/material/ic_info_outline.svg"
                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    helpDialog.title = qsTr("ICAO Type")
                    helpDialog.text = "<p>"
                            + qsTr("Enter the ICAO aircraft type designator of your aircraft (e.g., C172 for a Cessna 172, PA28 for a Piper Cherokee). This is used for flight plan filing.")
                            + "</p>"
                    helpDialog.open()
                }
            }

            CheckBox {
                id: pressureCheckBox

                Layout.columnSpan: 3
                Layout.fillWidth: true

                enabled: Sensors.pressureAltitude.isFinite()
                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    if (checked)
                        pressureWarning.open()
                    else
                        Navigator.aircraft.cabinPressureEqualsStaticPressure = false
                }
                checked: Navigator.aircraft.cabinPressureEqualsStaticPressure
                text: qsTr("Use cabin pressure instead of static pressure for pressure altitude calculation.")
                Component.onCompleted: contentItem.wrapMode = Text.WordWrap
            }
            ToolButton {
                icon.source: "/icons/material/ic_info_outline.svg"
                onClicked: {
                    PlatformAdaptor.vibrateBrief()
                    helpDialog.title = qsTr("Cabin/Altitude Pressure")
                    helpDialog.text = "<p>"
                            + qsTr("If this option is checked, Enroute Flight Navigation will use the pressure sensor of your mobile device to measure the pressure altitude and determine vertical distances to airspaces. This option is only available if your device has a pressure sensor.")
                            + "</p><p>"
                            + qsTr("If available, Enroute Flight Navigation will always use the pressure altitude provided by an external traffic data receiver instead of the pressure altitude calculated from the pressure sensor of your mobile device.")
                            + "</p>"
                    helpDialog.open()
                }
            }

            // Units
            Label {
                text: qsTr("Units")
                Layout.columnSpan: 4
                Layout.topMargin: font.pixelSize/2
                font.pixelSize: acftTab.font.pixelSize*1.2
                font.bold: true
            }

            Label {
                text: qsTr("Horizontal")
                Layout.alignment: Qt.AlignBaseline
            }
            ComboBox {
                id: horizontalUOM
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                KeyNavigation.tab: verticalUOM

                currentIndex: {
                    if (Navigator.aircraft.horizontalDistanceUnit === Aircraft.Kilometer) {
                        return 1
                    }
                    if (Navigator.aircraft.horizontalDistanceUnit === Aircraft.StatuteMile) {
                        return 2
                    }
                    return 0
                }
                onActivated: Navigator.aircraft.horizontalDistanceUnit = currentIndex

                model: [ qsTr("Nautical Miles"), qsTr("Kilometers"), qsTr("Statute Miles") ]
            }
            Item { }

            Label {
                text: qsTr("Vertical")
                Layout.alignment: Qt.AlignBaseline
            }
            ComboBox {
                id: verticalUOM
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                KeyNavigation.tab: volumeUOM

                currentIndex: {
                    if (Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters) {
                        return 1
                    }
                    return 0
                }
                onActivated: Navigator.aircraft.verticalDistanceUnit = currentIndex

                model: [ qsTr("Feet"), qsTr("Meters") ]
            }
            Item { }

            Label {
                text: qsTr("Volume")
                Layout.alignment: Qt.AlignBaseline
            }
            ComboBox {
                id: volumeUOM
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                KeyNavigation.tab: cruiseSpeed

                currentIndex: {
                    if (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour) {
                        return 1
                    }
                    return 0
                }
                onActivated: Navigator.aircraft.fuelConsumptionUnit = currentIndex

                model: [ qsTr("Liters"), qsTr("U.S. Gallons") ]
            }
            Item { }

            // Performance
            Label {
                text: qsTr("Performance")
                Layout.columnSpan: 4
                Layout.topMargin: font.pixelSize
                font.pixelSize: acftTab.font.pixelSize*1.2
                font.bold: true
            }

            // Column headers
            Item { Layout.fillWidth: true }
            Label {
                text: qsTr("Hor. Speed") + "\n(" + (Navigator.aircraft.horizontalDistanceUnit === Aircraft.Kilometer ? "km/h" : Navigator.aircraft.horizontalDistanceUnit === Aircraft.StatuteMile ? "mph" : "kn") + ")"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.NoWrap
            }
            Label {
                text: qsTr("Vert. Speed") + "\n(" + (Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters ? "m/s" : "ft/min") + ")"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.NoWrap
            }
            Label {
                text: qsTr("Fuel") + "\n(" + (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour ? "gal/h" : "l/h") + ")"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.NoWrap
            }

            // Cruise row
            Label { text: qsTr("Cruise"); Layout.alignment: Qt.AlignBaseline }
            MyTextField {
                id: cruiseSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhDigitsOnly
                validator: DoubleValidator { bottom: Navigator.aircraft.minValidSpeed.toKN(); top: Navigator.aircraft.maxValidSpeed.toKN(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  Navigator.aircraft.cruiseSpeed = aircraftPage.staticSpeed.fromKN(Number.fromLocaleString(Qt.locale(), text));  break
                    case Aircraft.Kilometer:     Navigator.aircraft.cruiseSpeed = aircraftPage.staticSpeed.fromKMH(Number.fromLocaleString(Qt.locale(), text)); break
                    case Aircraft.StatuteMile:   Navigator.aircraft.cruiseSpeed = aircraftPage.staticSpeed.fromMPH(Number.fromLocaleString(Qt.locale(), text)); break
                    }
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.cruiseSpeed.isFinite()) return ""
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  return Math.round(Navigator.aircraft.cruiseSpeed.toKN()).toString()
                    case Aircraft.Kilometer:     return Math.round(Navigator.aircraft.cruiseSpeed.toKMH()).toString()
                    case Aircraft.StatuteMile:   return Math.round(Navigator.aircraft.cruiseSpeed.toMPH()).toString()
                    }
                }
            }
            Item { }
            MyTextField {
                id: cruiseFuel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: 0; top: Navigator.aircraft.maxValidFuelConsumption.toLPH(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour)
                        Navigator.aircraft.fuelConsumption = aircraftPage.staticVolumeFlow.fromGPH(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.fuelConsumption = aircraftPage.staticVolumeFlow.fromLPH(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.fuelConsumption.isFinite()) return ""
                    return Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour
                        ? Navigator.aircraft.fuelConsumption.toGPH().toLocaleString(Qt.locale(), "f", 1)
                        : Navigator.aircraft.fuelConsumption.toLPH().toLocaleString(Qt.locale(), "f", 1)
                }
            }

            // Climb row
            Label { id: colorLabel; text: qsTr("Climb"); Layout.alignment: Qt.AlignBaseline }
            MyTextField {
                id: climbSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhDigitsOnly
                validator: DoubleValidator { bottom: Navigator.aircraft.minValidSpeed.toKN(); top: Navigator.aircraft.maxValidSpeed.toKN(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  Navigator.aircraft.climbSpeed = aircraftPage.staticSpeed.fromKN(Number.fromLocaleString(Qt.locale(), text));  break
                    case Aircraft.Kilometer:     Navigator.aircraft.climbSpeed = aircraftPage.staticSpeed.fromKMH(Number.fromLocaleString(Qt.locale(), text)); break
                    case Aircraft.StatuteMile:   Navigator.aircraft.climbSpeed = aircraftPage.staticSpeed.fromMPH(Number.fromLocaleString(Qt.locale(), text)); break
                    }
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.climbSpeed.isFinite()) return ""
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  return Math.round(Navigator.aircraft.climbSpeed.toKN()).toString()
                    case Aircraft.Kilometer:     return Math.round(Navigator.aircraft.climbSpeed.toKMH()).toString()
                    case Aircraft.StatuteMile:   return Math.round(Navigator.aircraft.climbSpeed.toMPH()).toString()
                    }
                }
            }
            MyTextField {
                id: climbVerticalSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhDigitsOnly
                validator: DoubleValidator { bottom: 0; top: Navigator.aircraft.maxValidVerticalSpeed.toFPM(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters)
                        Navigator.aircraft.climbVerticalSpeed = aircraftPage.staticSpeed.fromMPS(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.climbVerticalSpeed = aircraftPage.staticSpeed.fromFPM(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.climbVerticalSpeed.isFinite()) return ""
                    return Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters
                        ? Navigator.aircraft.climbVerticalSpeed.toMPS().toLocaleString(Qt.locale(), "f", 1)
                        : Math.round(Navigator.aircraft.climbVerticalSpeed.toFPM()).toString()
                }
            }
            MyTextField {
                id: climbFuel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: 0; top: Navigator.aircraft.maxValidFuelConsumption.toLPH(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour)
                        Navigator.aircraft.climbFuelConsumption = aircraftPage.staticVolumeFlow.fromGPH(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.climbFuelConsumption = aircraftPage.staticVolumeFlow.fromLPH(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.climbFuelConsumption.isFinite()) return ""
                    return Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour
                        ? Navigator.aircraft.climbFuelConsumption.toGPH().toLocaleString(Qt.locale(), "f", 1)
                        : Navigator.aircraft.climbFuelConsumption.toLPH().toLocaleString(Qt.locale(), "f", 1)
                }
            }

            // Descent row
            Label { text: qsTr("Descent"); Layout.alignment: Qt.AlignBaseline }
            MyTextField {
                id: descentSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhDigitsOnly
                validator: DoubleValidator { bottom: Navigator.aircraft.minValidSpeed.toKN(); top: Navigator.aircraft.maxValidSpeed.toKN(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  Navigator.aircraft.descentSpeed = aircraftPage.staticSpeed.fromKN(Number.fromLocaleString(Qt.locale(), text));  break
                    case Aircraft.Kilometer:     Navigator.aircraft.descentSpeed = aircraftPage.staticSpeed.fromKMH(Number.fromLocaleString(Qt.locale(), text)); break
                    case Aircraft.StatuteMile:   Navigator.aircraft.descentSpeed = aircraftPage.staticSpeed.fromMPH(Number.fromLocaleString(Qt.locale(), text)); break
                    }
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.descentSpeed.isFinite()) return ""
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  return Math.round(Navigator.aircraft.descentSpeed.toKN()).toString()
                    case Aircraft.Kilometer:     return Math.round(Navigator.aircraft.descentSpeed.toKMH()).toString()
                    case Aircraft.StatuteMile:   return Math.round(Navigator.aircraft.descentSpeed.toMPH()).toString()
                    }
                }
            }
            MyTextField {
                id: descentVerticalSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: Navigator.aircraft.minValidVerticalSpeed.toFPM(); top: Navigator.aircraft.maxValidVerticalSpeed.toFPM(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters)
                        Navigator.aircraft.descentVerticalSpeed = aircraftPage.staticSpeed.fromMPS(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.descentVerticalSpeed = aircraftPage.staticSpeed.fromFPM(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.descentVerticalSpeed.isFinite()) return ""
                    return Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters
                        ? Navigator.aircraft.descentVerticalSpeed.toMPS().toLocaleString(Qt.locale(), "f", 1)
                        : Math.round(Navigator.aircraft.descentVerticalSpeed.toFPM()).toString()
                }
            }
            MyTextField {
                id: descentFuel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: 0; top: Navigator.aircraft.maxValidFuelConsumption.toLPH(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour)
                        Navigator.aircraft.descentFuelConsumption = aircraftPage.staticVolumeFlow.fromGPH(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.descentFuelConsumption = aircraftPage.staticVolumeFlow.fromLPH(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.descentFuelConsumption.isFinite()) return ""
                    return Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour
                        ? Navigator.aircraft.descentFuelConsumption.toGPH().toLocaleString(Qt.locale(), "f", 1)
                        : Navigator.aircraft.descentFuelConsumption.toLPH().toLocaleString(Qt.locale(), "f", 1)
                }
            }

            // Glide row
            Label { text: qsTr("Glide"); Layout.alignment: Qt.AlignBaseline }
            MyTextField {
                id: glideSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhDigitsOnly
                validator: DoubleValidator { bottom: Navigator.aircraft.minValidSpeed.toKN(); top: Navigator.aircraft.maxValidSpeed.toKN(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  Navigator.aircraft.glideSpeed = aircraftPage.staticSpeed.fromKN(Number.fromLocaleString(Qt.locale(), text));  break
                    case Aircraft.Kilometer:     Navigator.aircraft.glideSpeed = aircraftPage.staticSpeed.fromKMH(Number.fromLocaleString(Qt.locale(), text)); break
                    case Aircraft.StatuteMile:   Navigator.aircraft.glideSpeed = aircraftPage.staticSpeed.fromMPH(Number.fromLocaleString(Qt.locale(), text)); break
                    }
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.glideSpeed.isFinite()) return ""
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  return Math.round(Navigator.aircraft.glideSpeed.toKN()).toString()
                    case Aircraft.Kilometer:     return Math.round(Navigator.aircraft.glideSpeed.toKMH()).toString()
                    case Aircraft.StatuteMile:   return Math.round(Navigator.aircraft.glideSpeed.toMPH()).toString()
                    }
                }
            }
            MyTextField {
                id: glideVerticalSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: Navigator.aircraft.minValidVerticalSpeed.toFPM(); top: Navigator.aircraft.maxValidVerticalSpeed.toFPM(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters)
                        Navigator.aircraft.glideVerticalSpeed = aircraftPage.staticSpeed.fromMPS(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.glideVerticalSpeed = aircraftPage.staticSpeed.fromFPM(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.glideVerticalSpeed.isFinite()) return ""
                    return Navigator.aircraft.verticalDistanceUnit === Aircraft.Meters
                        ? Navigator.aircraft.glideVerticalSpeed.toMPS().toLocaleString(Qt.locale(), "f", 1)
                        : Math.round(Navigator.aircraft.glideVerticalSpeed.toFPM()).toString()
                }
            }
            Item { }

            // Minimum speed (stall – used for flight/ground detection)
            Label {
                text: qsTr("Min. Speed")
                Layout.alignment: Qt.AlignBaseline
                Layout.topMargin: font.pixelSize/2
            }
            MyTextField {
                id: minimumSpeed
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*4
                Layout.topMargin: font.pixelSize/2
                inputMethodHints: Qt.ImhDigitsOnly
                validator: DoubleValidator { bottom: Navigator.aircraft.minValidSpeed.toKN(); top: Navigator.aircraft.maxValidSpeed.toKN(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  Navigator.aircraft.minimumSpeed = aircraftPage.staticSpeed.fromKN(Number.fromLocaleString(Qt.locale(), text));  break
                    case Aircraft.Kilometer:     Navigator.aircraft.minimumSpeed = aircraftPage.staticSpeed.fromKMH(Number.fromLocaleString(Qt.locale(), text)); break
                    case Aircraft.StatuteMile:   Navigator.aircraft.minimumSpeed = aircraftPage.staticSpeed.fromMPH(Number.fromLocaleString(Qt.locale(), text)); break
                    }
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.minimumSpeed.isFinite()) return ""
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  return Math.round(Navigator.aircraft.minimumSpeed.toKN()).toString()
                    case Aircraft.Kilometer:     return Math.round(Navigator.aircraft.minimumSpeed.toKMH()).toString()
                    case Aircraft.StatuteMile:   return Math.round(Navigator.aircraft.minimumSpeed.toMPH()).toString()
                    }
                }
            }
            Label {
                Layout.alignment: Qt.AlignBaseline
                Layout.topMargin: font.pixelSize/2
                text: {
                    switch(Navigator.aircraft.horizontalDistanceUnit) {
                    case Aircraft.NauticalMile:  return "kn"
                    case Aircraft.Kilometer:     return "km/h"
                    case Aircraft.StatuteMile:   return "mph"
                    }
                }
            }
            Item { }


            // Fuel Capacity
            Label {
                text: qsTr("Fuel Capacity")
                Layout.columnSpan: 4
                Layout.topMargin: font.pixelSize
                font.pixelSize: acftTab.font.pixelSize*1.2
                font.bold: true
            }

            Label { text: qsTr("Max. Capacity"); Layout.alignment: Qt.AlignBaseline }
            MyTextField {
                id: maxFuelCapacity
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*5
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: 0; top: Navigator.aircraft.maxValidFuelVolume.toL(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour)
                        Navigator.aircraft.maxFuelCapacity = aircraftPage.staticVolume.fromGAL(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.maxFuelCapacity = aircraftPage.staticVolume.fromL(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.maxFuelCapacity.isFinite()) return ""
                    return Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour
                        ? Navigator.aircraft.maxFuelCapacity.toGAL().toLocaleString(Qt.locale(), "f", 1)
                        : Navigator.aircraft.maxFuelCapacity.toL().toLocaleString(Qt.locale(), "f", 1)
                }
            }
            Label {
                Layout.alignment: Qt.AlignBaseline
                text: Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour ? "gal" : "l"
            }

            Label { text: qsTr("Not Useable"); Layout.alignment: Qt.AlignBaseline }
            MyTextField {
                id: unusableFuel
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*5
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: 0; top: Navigator.aircraft.maxValidFuelVolume.toL(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour)
                        Navigator.aircraft.unusableFuel = aircraftPage.staticVolume.fromGAL(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.unusableFuel = aircraftPage.staticVolume.fromL(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.unusableFuel.isFinite()) return ""
                    return Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour
                        ? Navigator.aircraft.unusableFuel.toGAL().toLocaleString(Qt.locale(), "f", 1)
                        : Navigator.aircraft.unusableFuel.toL().toLocaleString(Qt.locale(), "f", 1)
                }
            }
            Label {
                Layout.alignment: Qt.AlignBaseline
                text: Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour ? "gal" : "l"
            }

            Label { text: qsTr("Additional Fuel"); Layout.alignment: Qt.AlignBaseline }
            MyTextField {
                id: additionalFuel
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                Layout.minimumWidth: font.pixelSize*5
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { bottom: 0; top: Navigator.aircraft.maxValidFuelVolume.toL(); notation: DoubleValidator.StandardNotation }
                onEditingFinished: {
                    if (Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour)
                        Navigator.aircraft.additionalFuel = aircraftPage.staticVolume.fromGAL(Number.fromLocaleString(Qt.locale(), text))
                    else
                        Navigator.aircraft.additionalFuel = aircraftPage.staticVolume.fromL(Number.fromLocaleString(Qt.locale(), text))
                }
                color: acceptableInput ? colorLabel.color : "red"
                text: {
                    if (!Navigator.aircraft.additionalFuel.isFinite()) return ""
                    return Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour
                        ? Navigator.aircraft.additionalFuel.toGAL().toLocaleString(Qt.locale(), "f", 1)
                        : Navigator.aircraft.additionalFuel.toL().toLocaleString(Qt.locale(), "f", 1)
                }
            }
            Label {
                Layout.alignment: Qt.AlignBaseline
                text: Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour ? "gal" : "l"
            }

            Label { text: qsTr("Max Avail. to Flight"); Layout.alignment: Qt.AlignBaseline }
            TextField {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
                readOnly: true
                color: colorLabel.color
                text: {
                    var use = Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour
                    var cap = use ? Navigator.aircraft.maxFuelCapacity.toGAL() : Navigator.aircraft.maxFuelCapacity.toL()
                    if (isNaN(cap)) return ""
                    var unuse = use ? Navigator.aircraft.unusableFuel.toGAL() : Navigator.aircraft.unusableFuel.toL()
                    var add = use ? Navigator.aircraft.additionalFuel.toGAL() : Navigator.aircraft.additionalFuel.toL()
                    return (cap - (isNaN(unuse) ? 0 : unuse) + (isNaN(add) ? 0 : add)).toLocaleString(Qt.locale(), "f", 1)
                }
            }
            Label {
                Layout.alignment: Qt.AlignBaseline
                text: Navigator.aircraft.fuelConsumptionUnit === Aircraft.GallonPerHour ? "gal" : "l"
            }

            Rectangle {
                Layout.columnSpan: 4
                Layout.preferredHeight: acftTab.font.pixelSize
            }


    }

    }

        // Tab 2: FPL
        Item { }

        // Tab 3: W & B
        DecoratedScrollView {
            id: wbTab
            contentWidth: width
            clip: true

            GridLayout {
                anchors.left: parent.left
                anchors.leftMargin: wbTab.font.pixelSize
                anchors.right: parent.right
                anchors.rightMargin: wbTab.font.pixelSize
                columns: 3

                // Units
                Label {
                    text: qsTr("Units")
                    Layout.columnSpan: 3
                    Layout.topMargin: font.pixelSize/2
                    font.pixelSize: wbTab.font.pixelSize*1.2
                    font.bold: true
                }

                Label { text: qsTr("Weight"); Layout.alignment: Qt.AlignBaseline }
                ComboBox {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignBaseline
                    currentIndex: Navigator.aircraft.wbWeightUnit
                    model: ["kg", "lbs"]
                    onActivated: Navigator.aircraft.wbWeightUnit = currentIndex
                }

                Label { text: qsTr("Length"); Layout.alignment: Qt.AlignBaseline }
                ComboBox {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignBaseline
                    currentIndex: Navigator.aircraft.wbLengthUnit
                    model: ["mm", "m", "inch"]
                    onActivated: Navigator.aircraft.wbLengthUnit = currentIndex
                }

                // W&B table
                Label {
                    text: qsTr("Weight && Balance")
                    Layout.columnSpan: 3
                    Layout.topMargin: wbTab.font.pixelSize/2
                    font.pixelSize: wbTab.font.pixelSize*1.2
                    font.bold: true
                }

                // Column headers
                Label { text: qsTr("Station"); Layout.fillWidth: true; font.bold: true }
                Label {
                    text: qsTr("Max Mass") + "
(" + (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? "lbs" : "kg") + ")"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                }
                Label {
                    text: qsTr("Arm") + "
(" + (Navigator.aircraft.wbLengthUnit === Aircraft.WBMeter ? "m" : Navigator.aircraft.wbLengthUnit === Aircraft.WBInch ? "inch" : "mm") + ")"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                }

                // Empty Mass
                Label { text: qsTr("Empty Mass"); Layout.alignment: Qt.AlignBaseline }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        Navigator.aircraft.wbEmptyMass = Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? v * 0.453592 : v
                    }
                    text: {
                        var kg = Navigator.aircraft.wbEmptyMass
                        if (isNaN(kg)) return ""
                        return (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? kg * 2.20462 : kg).toLocaleString(Qt.locale(), "f", 1)
                    }
                }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        var m
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) m = v / 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) m = v * 0.0254
                        else m = v
                        Navigator.aircraft.wbEmptyArm = m
                    }
                    text: {
                        var m = Navigator.aircraft.wbEmptyArm
                        if (isNaN(m)) return ""
                        var v
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) v = m * 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) v = m / 0.0254
                        else v = m
                        return v.toLocaleString(Qt.locale(), "f", 1)
                    }
                }

                // Pilot and Front Pax
                Label { text: qsTr("Pilot and Front Pax"); Layout.alignment: Qt.AlignBaseline }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        Navigator.aircraft.wbPilotFrontMaxMass = Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? v * 0.453592 : v
                    }
                    text: {
                        var kg = Navigator.aircraft.wbPilotFrontMaxMass
                        if (isNaN(kg)) return ""
                        return (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? kg * 2.20462 : kg).toLocaleString(Qt.locale(), "f", 1)
                    }
                }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        var m
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) m = v / 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) m = v * 0.0254
                        else m = v
                        Navigator.aircraft.wbPilotFrontArm = m
                    }
                    text: {
                        var m = Navigator.aircraft.wbPilotFrontArm
                        if (isNaN(m)) return ""
                        var v
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) v = m * 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) v = m / 0.0254
                        else v = m
                        return v.toLocaleString(Qt.locale(), "f", 1)
                    }
                }

                // Rear Pax
                Label { text: qsTr("Rear Pax"); Layout.alignment: Qt.AlignBaseline }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        Navigator.aircraft.wbRearPaxMaxMass = Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? v * 0.453592 : v
                    }
                    text: {
                        var kg = Navigator.aircraft.wbRearPaxMaxMass
                        if (isNaN(kg)) return ""
                        return (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? kg * 2.20462 : kg).toLocaleString(Qt.locale(), "f", 1)
                    }
                }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        var m
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) m = v / 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) m = v * 0.0254
                        else m = v
                        Navigator.aircraft.wbRearPaxArm = m
                    }
                    text: {
                        var m = Navigator.aircraft.wbRearPaxArm
                        if (isNaN(m)) return ""
                        var v
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) v = m * 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) v = m / 0.0254
                        else v = m
                        return v.toLocaleString(Qt.locale(), "f", 1)
                    }
                }

                // Cargo
                Label { text: qsTr("Cargo"); Layout.alignment: Qt.AlignBaseline }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        Navigator.aircraft.wbCargoMaxMass = Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? v * 0.453592 : v
                    }
                    text: {
                        var kg = Navigator.aircraft.wbCargoMaxMass
                        if (isNaN(kg)) return ""
                        return (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? kg * 2.20462 : kg).toLocaleString(Qt.locale(), "f", 1)
                    }
                }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        var m
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) m = v / 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) m = v * 0.0254
                        else m = v
                        Navigator.aircraft.wbCargoArm = m
                    }
                    text: {
                        var m = Navigator.aircraft.wbCargoArm
                        if (isNaN(m)) return ""
                        var v
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) v = m * 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) v = m / 0.0254
                        else v = m
                        return v.toLocaleString(Qt.locale(), "f", 1)
                    }
                }

                // Fuel
                Label { text: qsTr("Fuel"); Layout.alignment: Qt.AlignBaseline }
                TextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    readOnly: true
                    color: colorLabel.color
                    text: {
                        var capL = Navigator.aircraft.maxFuelCapacity.toL()
                        if (isNaN(capL)) return ""
                        var kg = capL * 0.72
                        return (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? kg * 2.20462 : kg).toLocaleString(Qt.locale(), "f", 1)
                    }
                }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        var m
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) m = v / 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) m = v * 0.0254
                        else m = v
                        Navigator.aircraft.wbFuelArm = m
                    }
                    text: {
                        var m = Navigator.aircraft.wbFuelArm
                        if (isNaN(m)) return ""
                        var v
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) v = m * 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) v = m / 0.0254
                        else v = m
                        return v.toLocaleString(Qt.locale(), "f", 1)
                    }
                }

                // Oil
                Label { text: qsTr("Oil"); Layout.alignment: Qt.AlignBaseline }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        Navigator.aircraft.wbOilMaxMass = Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? v * 0.453592 : v
                    }
                    text: {
                        var kg = Navigator.aircraft.wbOilMaxMass
                        if (isNaN(kg)) return ""
                        return (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? kg * 2.20462 : kg).toLocaleString(Qt.locale(), "f", 1)
                    }
                }
                MyTextField {
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignBaseline; Layout.minimumWidth: font.pixelSize*4
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        var v = Number.fromLocaleString(Qt.locale(), text)
                        var m
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) m = v / 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) m = v * 0.0254
                        else m = v
                        Navigator.aircraft.wbOilArm = m
                    }
                    text: {
                        var m = Navigator.aircraft.wbOilArm
                        if (isNaN(m)) return ""
                        var v
                        if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) v = m * 1000
                        else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) v = m / 0.0254
                        else v = m
                        return v.toLocaleString(Qt.locale(), "f", 1)
                    }
                }

                // CG Envelope
                Label {
                    text: qsTr("CG Envelope")
                    Layout.columnSpan: 3
                    Layout.topMargin: wbTab.font.pixelSize/2
                    font.pixelSize: wbTab.font.pixelSize*1.2
                    font.bold: true
                }

                ColumnLayout {
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    spacing: 0

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: qsTr("Mass") + "
(" + (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? "lbs" : "kg") + ")"
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.bold: true
                        }
                        Label {
                            text: qsTr("Arm") + "
(" + (Navigator.aircraft.wbLengthUnit === Aircraft.WBMeter ? "m" : Navigator.aircraft.wbLengthUnit === Aircraft.WBInch ? "inch" : "mm") + ")"
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.bold: true
                        }
                    }

                    Repeater {
                        model: Navigator.aircraft.wbEnvMasses.length
                        delegate: RowLayout {
                            Layout.fillWidth: true
                            property int rowIndex: index

                            MyTextField {
                                Layout.fillWidth: true
                                Layout.minimumWidth: font.pixelSize*4
                                inputMethodHints: Qt.ImhFormattedNumbersOnly
                                onEditingFinished: {
                                    var val = Number.fromLocaleString(Qt.locale(), text)
                                    var kg = Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? val * 0.453592 : val
                                    var masses = Navigator.aircraft.wbEnvMasses
                                    masses[rowIndex] = kg
                                    Navigator.aircraft.wbEnvMasses = masses
                                }
                                text: {
                                    var kg = Navigator.aircraft.wbEnvMasses[rowIndex]
                                    if (kg === undefined || isNaN(kg)) return ""
                                    return (Navigator.aircraft.wbWeightUnit === Aircraft.WBPound ? kg * 2.20462 : kg).toLocaleString(Qt.locale(), "f", 1)
                                }
                            }

                            MyTextField {
                                Layout.fillWidth: true
                                Layout.minimumWidth: font.pixelSize*4
                                inputMethodHints: Qt.ImhFormattedNumbersOnly
                                onEditingFinished: {
                                    var val = Number.fromLocaleString(Qt.locale(), text)
                                    var m
                                    if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) m = val / 1000
                                    else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) m = val * 0.0254
                                    else m = val
                                    var arms = Navigator.aircraft.wbEnvArms
                                    arms[rowIndex] = m
                                    Navigator.aircraft.wbEnvArms = arms
                                }
                                text: {
                                    var m = Navigator.aircraft.wbEnvArms[rowIndex]
                                    if (m === undefined || isNaN(m)) return ""
                                    var v
                                    if (Navigator.aircraft.wbLengthUnit === Aircraft.WBMillimeter) v = m * 1000
                                    else if (Navigator.aircraft.wbLengthUnit === Aircraft.WBInch) v = m / 0.0254
                                    else v = m
                                    return v.toLocaleString(Qt.locale(), "f", 1)
                                }
                            }
                        }
                    }

                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: wbTab.font.pixelSize/2
                        text: qsTr("Add Envelope Edge")
                        enabled: Navigator.aircraft.wbEnvMasses.length < 7
                        onClicked: {
                            PlatformAdaptor.vibrateBrief()
                            var masses = Navigator.aircraft.wbEnvMasses
                            var arms = Navigator.aircraft.wbEnvArms
                            masses.push(NaN)
                            arms.push(NaN)
                            Navigator.aircraft.wbEnvMasses = masses
                            Navigator.aircraft.wbEnvArms = arms
                        }
                    }
                }

                Rectangle {
                    Layout.columnSpan: 3
                    Layout.preferredHeight: wbTab.font.pixelSize
                }
            }
        }

    } // SwipeView


    Loader {
        id: dlgLoader
        anchors.fill: parent

        onLoaded: {
            item.modal = true
            item.open()
        }
    }

    LongTextDialog {
        id: pressureWarning

        modal: true
        title: qsTr("Flight Safety Warning!")
        text: "<p>"
              + "<strong>" + qsTr("Know what you are doing!") + " </strong>"
              + qsTr("If you enable this option, Enroute Flight Navigation will use the pressure sensor of your mobile device to determine pressure altitude and vertical distances to airspaces.") + " "
              + qsTr("This is safety critical.") + " "
              + "</p>"
              + "<p><ul style='margin-left:-25px;'>"
              + "<li>" + qsTr("The pressure sensor of your device is probably not certified for use in aviation.") + "</li>"
              + "<li>" + qsTr("In typical GA aircraft, cabin pressure is not equal to static pressure.") + " "
              + qsTr("The precise difference depend on factors such as airspeed and configuration of ventilation and heating.") + "</li>"
              + "</ul></p><p>"
              + qsTr("Do not enable this option unless you convinced yourself that the data provided by your sensor is good enough for the intended use.") + " "
              + qsTr("Do not rely on data shown in this app.") + " "
              + qsTr("Always use an approved altimeter to judge vertical distance to airspaces.") + " "
              + "</p>"
              + "<p>"
              + qsTr("We strongly recommend to connect Enroute Flight Navigation to a proper traffic data receiver, such as a FLARM or ADS-B device, that provides static pressure data.")
              + "</p>"
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            PlatformAdaptor.vibrateBrief()
            Navigator.aircraft.cabinPressureEqualsStaticPressure = true
        }

        onRejected: {
            PlatformAdaptor.vibrateBrief()
            pressureCheckBox.checked = false
        }
    }

    LongTextDialog {
        id: helpDialog

        modal: true

        standardButtons: Dialog.Ok
    }

} // Page
