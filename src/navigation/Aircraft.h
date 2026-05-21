/***************************************************************************
 *   Copyright (C) 2019-2022 by Stefan Kebekus                             *
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

#pragma once

#include "units/Distance.h"
#include "units/Speed.h"
#include "units/Volume.h"
#include "units/VolumeFlow.h"

#include <QGeoCoordinate>
#include <QQmlEngine>
#include <QVariantList>

namespace Navigation {

/*! \brief This extremely simple class holds a few numbers that describe an aircraft */

class Aircraft {
    Q_GADGET
    QML_VALUE_TYPE(aircraft)

public:
    /*! \brief Units of measurement for volumes */
    enum FuelConsumptionUnit : quint8 {
        /*! \brief Liter per hour */
        LiterPerHour = 0,

        /*! \brief Gallon per hour */
        GallonPerHour = 1
    };
    Q_ENUM(FuelConsumptionUnit)

    /*! \brief Units of measurement for horizontal distances */
    enum HorizontalDistanceUnit : quint8 {
        /*! \brief Nautical Mile */
        NauticalMile = 0,

        /*! \brief Kilometer */
        Kilometer = 1,

        /*! \brief Statute Mile */
        StatuteMile = 2
    };
    Q_ENUM(HorizontalDistanceUnit)

    /*! \brief Units of measurement for vertical distances */
    enum VerticalDistanceUnit : quint8 {
        /*! \brief Feet */
        Feet = 0,

        /*! \brief Meters */
        Meters = 1
    };
    Q_ENUM(VerticalDistanceUnit)

    /*! \brief Weight unit for Weight & Balance */
    enum WBWeightUnit : quint8 {
        WBKilogram = 0,
        WBPound = 1
    };
    Q_ENUM(WBWeightUnit)

    /*! \brief Length/arm unit for Weight & Balance */
    enum WBLengthUnit : quint8 {
        WBMillimeter = 0,
        WBMeter = 1,
        WBInch = 2
    };
    Q_ENUM(WBLengthUnit)


    //
    // Properties
    //

    /*! \brief Preferred units of measurement for vertical distances */
    Q_PROPERTY(bool cabinPressureEqualsStaticPressure READ cabinPressureEqualsStaticPressure WRITE setCabinPressureEqualsStaticPressure)

    /*! \brief Climb Speed (horizontal TAS) */
    Q_PROPERTY(Units::Speed climbSpeed READ climbSpeed WRITE setClimbSpeed)

    /*! \brief Climb Vertical Speed */
    Q_PROPERTY(Units::Speed climbVerticalSpeed READ climbVerticalSpeed WRITE setClimbVerticalSpeed)

    /*! \brief Fuel Consumption during Climb */
    Q_PROPERTY(Units::VolumeFlow climbFuelConsumption READ climbFuelConsumption WRITE setClimbFuelConsumption)

    /*! \brief Cruise Speed (horizontal TAS) */
    Q_PROPERTY(Units::Speed cruiseSpeed READ cruiseSpeed WRITE setCruiseSpeed)

    /*! \brief Cruise Vertical Speed */
    Q_PROPERTY(Units::Speed cruiseVerticalSpeed READ cruiseVerticalSpeed WRITE setCruiseVerticalSpeed)

    /*! \brief Fuel Consumption during Cruise */
    Q_PROPERTY(Units::VolumeFlow fuelConsumption READ fuelConsumption WRITE setFuelConsumption)

    /*! \brief Descent Speed (horizontal TAS) */
    Q_PROPERTY(Units::Speed descentSpeed READ descentSpeed WRITE setDescentSpeed)

    /*! \brief Descent Vertical Speed */
    Q_PROPERTY(Units::Speed descentVerticalSpeed READ descentVerticalSpeed WRITE setDescentVerticalSpeed)

    /*! \brief Fuel Consumption during Descent */
    Q_PROPERTY(Units::VolumeFlow descentFuelConsumption READ descentFuelConsumption WRITE setDescentFuelConsumption)

    /*! \brief Glide Speed (horizontal TAS) */
    Q_PROPERTY(Units::Speed glideSpeed READ glideSpeed WRITE setGlideSpeed)

    /*! \brief Glide Vertical Speed (sink rate) */
    Q_PROPERTY(Units::Speed glideVerticalSpeed READ glideVerticalSpeed WRITE setGlideVerticalSpeed)

    /*! \brief Maximum fuel capacity (total tank volume) */
    Q_PROPERTY(Units::Volume maxFuelCapacity READ maxFuelCapacity WRITE setMaxFuelCapacity)

    /*! \brief Unusable fuel (trapped, cannot be burned) */
    Q_PROPERTY(Units::Volume unusableFuel READ unusableFuel WRITE setUnusableFuel)

    /*! \brief Additional fuel (auxiliary tanks, ferry fuel, etc.) */
    Q_PROPERTY(Units::Volume additionalFuel READ additionalFuel WRITE setAdditionalFuel)

    /*! \brief Maximal fuel volume that is considered valid */
    Q_PROPERTY(Units::Volume maxValidFuelVolume MEMBER maxValidFuelVolume CONSTANT)

    /*! \brief Preferred units of measurement for fuel consumption */
    Q_PROPERTY(FuelConsumptionUnit fuelConsumptionUnit READ fuelConsumptionUnit WRITE setFuelConsumptionUnit)

    /*! \brief Preferred units of measurement for horizontal distances */
    Q_PROPERTY(HorizontalDistanceUnit horizontalDistanceUnit READ horizontalDistanceUnit WRITE setHorizontalDistanceUnit)

    /*! \brief Maximal speed of the aircraft that is considered valid */
    Q_PROPERTY(Units::Speed maxValidSpeed MEMBER maxValidSpeed CONSTANT)

    /*! \brief Maximal vertical speed that is considered valid */
    Q_PROPERTY(Units::Speed maxValidVerticalSpeed MEMBER maxValidVerticalSpeed CONSTANT)

    /*! \brief Maximal fuel consumption that is considered valid */
    Q_PROPERTY(Units::VolumeFlow maxValidFuelConsumption MEMBER maxValidFuelConsumption CONSTANT)

    /*! \brief Minimum Speed (stall speed, used for flight/ground detection) */
    Q_PROPERTY(Units::Speed minimumSpeed READ minimumSpeed WRITE setMinimumSpeed)

    /*! \brief Minimal fuel consumption that is considered valid */
    Q_PROPERTY(Units::VolumeFlow minValidFuelConsumption MEMBER minValidFuelConsumption CONSTANT)

    /*! \brief Minimal speed of the aircraft that is considered valid */
    Q_PROPERTY(Units::Speed minValidSpeed MEMBER minValidSpeed CONSTANT)

    /*! \brief Minimal vertical speed that is considered valid */
    Q_PROPERTY(Units::Speed minValidVerticalSpeed MEMBER minValidVerticalSpeed CONSTANT)

    /*! \brief Name
     *
     * This property holds the name.
     */
    Q_PROPERTY(QString name READ name WRITE setName)

    /*! \brief ICAO Aircraft Type Designator
     *
     * This property holds the ICAO aircraft type designator (e.g., "C172", "PA28").
     */
    Q_PROPERTY(QString icaoType READ icaoType WRITE setIcaoType)

    /*! \brief Transponder Code
     *
     * This property holds the ICAO 24-bit address (Mode S transponder code).
     * Typically a 6-character hexadecimal string (e.g., "3C6543").
     */
    Q_PROPERTY(QString transponderCode READ transponderCode WRITE setTransponderCode)

    /*! \brief Preferred units of measurement for vertical distances */
    Q_PROPERTY(VerticalDistanceUnit verticalDistanceUnit READ verticalDistanceUnit WRITE setVerticalDistanceUnit)

    /*! \brief Preferred weight unit for Weight & Balance */
    Q_PROPERTY(WBWeightUnit wbWeightUnit READ wbWeightUnit WRITE setWbWeightUnit)

    /*! \brief Preferred length/arm unit for Weight & Balance */
    Q_PROPERTY(WBLengthUnit wbLengthUnit READ wbLengthUnit WRITE setWbLengthUnit)

    // W&B station data (mass stored in kg, arm stored in m; NaN = not set)
    Q_PROPERTY(double wbEmptyMass READ wbEmptyMass WRITE setWbEmptyMass)
    Q_PROPERTY(double wbEmptyArm READ wbEmptyArm WRITE setWbEmptyArm)
    Q_PROPERTY(double wbPilotFrontMaxMass READ wbPilotFrontMaxMass WRITE setWbPilotFrontMaxMass)
    Q_PROPERTY(double wbPilotFrontArm READ wbPilotFrontArm WRITE setWbPilotFrontArm)
    Q_PROPERTY(double wbRearPaxMaxMass READ wbRearPaxMaxMass WRITE setWbRearPaxMaxMass)
    Q_PROPERTY(double wbRearPaxArm READ wbRearPaxArm WRITE setWbRearPaxArm)
    Q_PROPERTY(double wbCargoMaxMass READ wbCargoMaxMass WRITE setWbCargoMaxMass)
    Q_PROPERTY(double wbCargoArm READ wbCargoArm WRITE setWbCargoArm)
    Q_PROPERTY(double wbFuelMaxMass READ wbFuelMaxMass WRITE setWbFuelMaxMass)
    Q_PROPERTY(double wbFuelArm READ wbFuelArm WRITE setWbFuelArm)
    Q_PROPERTY(double wbOilMaxMass READ wbOilMaxMass WRITE setWbOilMaxMass)
    Q_PROPERTY(double wbOilArm READ wbOilArm WRITE setWbOilArm)

    // CG envelope: lists of up to 7 (mass_kg, arm_m) pairs
    Q_PROPERTY(QVariantList wbEnvMasses READ wbEnvMasses WRITE setWbEnvMasses)
    Q_PROPERTY(QVariantList wbEnvArms READ wbEnvArms WRITE setWbEnvArms)


    //
    // Getter Methods
    //

    /*! \brief Getter function for property of the same name
     *
     * @returns Property cabinPressureEqualsStaticPressure
     */
    [[nodiscard]] bool cabinPressureEqualsStaticPressure() const { return m_cabinPressureEqualsStaticPressure; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property cruise speed
     */
    [[nodiscard]] Units::Speed cruiseSpeed() const { return m_cruiseSpeed; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property descentSpeed
     */
    [[nodiscard]] Units::Speed descentSpeed() const { return m_descentSpeed; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property fuelConsumptionInLPH
     */
    [[nodiscard]] Units::VolumeFlow fuelConsumption() const { return m_fuelConsumption; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property preferredVolumeUnit
     */
    [[nodiscard]] FuelConsumptionUnit fuelConsumptionUnit() const { return m_fuelConsumptionUnit; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property preferredHorizontalDistanceUnit
     */
    [[nodiscard]] HorizontalDistanceUnit horizontalDistanceUnit() const { return m_horizontalDistanceUnit; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property minimum speed
     */
    [[nodiscard]] Units::Speed minimumSpeed() const { return m_minimumSpeed; }

    [[nodiscard]] Units::Volume maxFuelCapacity() const { return m_maxFuelCapacity; }
    [[nodiscard]] Units::Volume unusableFuel() const { return m_unusableFuel; }
    [[nodiscard]] Units::Volume additionalFuel() const { return m_additionalFuel; }
    [[nodiscard]] Units::Speed climbSpeed() const { return m_climbSpeed; }
    [[nodiscard]] Units::Speed climbVerticalSpeed() const { return m_climbVerticalSpeed; }
    [[nodiscard]] Units::VolumeFlow climbFuelConsumption() const { return m_climbFuelConsumption; }
    [[nodiscard]] Units::Speed cruiseVerticalSpeed() const { return m_cruiseVerticalSpeed; }
    [[nodiscard]] Units::Speed descentVerticalSpeed() const { return m_descentVerticalSpeed; }
    [[nodiscard]] Units::VolumeFlow descentFuelConsumption() const { return m_descentFuelConsumption; }
    [[nodiscard]] Units::Speed glideSpeed() const { return m_glideSpeed; }
    [[nodiscard]] Units::Speed glideVerticalSpeed() const { return m_glideVerticalSpeed; }

    [[nodiscard]] WBWeightUnit wbWeightUnit() const { return m_wbWeightUnit; }
    [[nodiscard]] WBLengthUnit wbLengthUnit() const { return m_wbLengthUnit; }
    [[nodiscard]] double wbEmptyMass() const { return m_wbEmptyMass; }
    [[nodiscard]] double wbEmptyArm() const { return m_wbEmptyArm; }
    [[nodiscard]] double wbPilotFrontMaxMass() const { return m_wbPilotFrontMaxMass; }
    [[nodiscard]] double wbPilotFrontArm() const { return m_wbPilotFrontArm; }
    [[nodiscard]] double wbRearPaxMaxMass() const { return m_wbRearPaxMaxMass; }
    [[nodiscard]] double wbRearPaxArm() const { return m_wbRearPaxArm; }
    [[nodiscard]] double wbCargoMaxMass() const { return m_wbCargoMaxMass; }
    [[nodiscard]] double wbCargoArm() const { return m_wbCargoArm; }
    [[nodiscard]] double wbFuelMaxMass() const { return m_wbFuelMaxMass; }
    [[nodiscard]] double wbFuelArm() const { return m_wbFuelArm; }
    [[nodiscard]] double wbOilMaxMass() const { return m_wbOilMaxMass; }
    [[nodiscard]] double wbOilArm() const { return m_wbOilArm; }
    [[nodiscard]] QVariantList wbEnvMasses() const { return m_wbEnvMasses; }
    [[nodiscard]] QVariantList wbEnvArms() const { return m_wbEnvArms; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property icaoType
     */
    [[nodiscard]] QString icaoType() const { return m_icaoType; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property name
     */
    [[nodiscard]] QString name() const { return m_name; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property transponderCode
     */
    [[nodiscard]] QString transponderCode() const { return m_transponderCode; }

    /*! \brief Getter function for property of the same name
     *
     * @returns Property preferredVertialDistanceUnit
     */
    [[nodiscard]] VerticalDistanceUnit verticalDistanceUnit() const { return m_verticalDistanceUnit; }


    //
    // Setter Methods
    //

    /*! \brief Setter function for property of the same name
     *
     * @param newCabinPressureEqualsStaticPressure Property cabinPressureEqualsStaticPressure
     */
    void setCabinPressureEqualsStaticPressure(bool newCabinPressureEqualsStaticPressure) { m_cabinPressureEqualsStaticPressure = newCabinPressureEqualsStaticPressure; }

    /*! \brief Setter function for property of the same name
     *
     * If newSpeed is
     * outside of the interval [minAircraftSpeed, maxAircraftSpeed], the
     * property will be set to NaN.
     *
     * @param newSpeed Property cruise speed
     */
    void setCruiseSpeed(Units::Speed newSpeed);

    /*! \brief Setter function for property of the same name
     *
     * @param newSpeed Descent speed
     */
    void setDescentSpeed(Units::Speed newSpeed);

    /*! \brief Setter function for property of the same name
     *
     * @param newFuelConsumption Fuel consumption
     */
    void setFuelConsumption(Units::VolumeFlow newFuelConsumption);

    /*! \brief Setter function for property of the same name
     *
     * @param newUnit Property preferredFuelConsumptionUnit
     */
    void setFuelConsumptionUnit(FuelConsumptionUnit newUnit);

    /*! \brief Setter function for property of the same name
     *
     * @param newUnit Property preferredHorizontalDistanceUnit
     */
    void setHorizontalDistanceUnit(HorizontalDistanceUnit newUnit);

    /*! \brief Setter function for property of the same name
     *
     * If newSpeed is outside of the interval [minAircraftSpeed, maxAircraftSpeed], the
     * property will be set to NaN.
     *
     * @param newSpeed Property minimum speed
     */
    void setMinimumSpeed(Units::Speed newSpeed);

    void setMaxFuelCapacity(Units::Volume newVolume);
    void setUnusableFuel(Units::Volume newVolume);
    void setAdditionalFuel(Units::Volume newVolume);
    void setClimbSpeed(Units::Speed newSpeed);
    void setClimbVerticalSpeed(Units::Speed newSpeed);
    void setClimbFuelConsumption(Units::VolumeFlow newFlow);
    void setCruiseVerticalSpeed(Units::Speed newSpeed);
    void setDescentVerticalSpeed(Units::Speed newSpeed);
    void setDescentFuelConsumption(Units::VolumeFlow newFlow);
    void setGlideSpeed(Units::Speed newSpeed);
    void setGlideVerticalSpeed(Units::Speed newSpeed);

    void setWbWeightUnit(WBWeightUnit u);
    void setWbLengthUnit(WBLengthUnit u);
    void setWbEmptyMass(double v);
    void setWbEmptyArm(double v);
    void setWbPilotFrontMaxMass(double v);
    void setWbPilotFrontArm(double v);
    void setWbRearPaxMaxMass(double v);
    void setWbRearPaxArm(double v);
    void setWbCargoMaxMass(double v);
    void setWbCargoArm(double v);
    void setWbFuelMaxMass(double v);
    void setWbFuelArm(double v);
    void setWbOilMaxMass(double v);
    void setWbOilArm(double v);
    void setWbEnvMasses(const QVariantList& v);
    void setWbEnvArms(const QVariantList& v);

    /*! \brief Setter function for property of the same name
     *
     * @param newIcaoType Property icaoType
     */
    void setIcaoType(const QString& newIcaoType);

    /*! \brief Setter function for property of the same name
     *
     * @param newName Property name
     */
    void setName(const QString& newName);

    /*! \brief Setter function for property of the same name
     *
     * @param newCode Property transponderCode
     */
    void setTransponderCode(const QString& newCode);

    /*! \brief Setter function for property of the same name
     *
     * @param newUnit Property preferredVerticalDistanceUnit
     */
    void setVerticalDistanceUnit(VerticalDistanceUnit newUnit);


    //
    // Methods
    //

    /*! \brief Return copy of this object
     *
     *  This is a helper function, used in QML to create explicit copies of this object.
     *
     *  @returns Copy of this object
     */
    [[nodiscard]] Q_INVOKABLE Navigation::Aircraft clone() const { return {*this}; }

    /*! \brief Description of the way between two points
     *
     * @param from Starting point of the way
     *
     * @param to Endpoint of the way
     *
     * @returns A string such as "DIST 65.2 nm • QUJ 276°" or "DIST 65.2 km • QUJ 276°".  If the way cannot be described (e.g. because one of the coordinates is invalid), then an empty string is returned.
     */
    [[nodiscard]] Q_INVOKABLE QString describeWay(const QGeoCoordinate &from, const QGeoCoordinate &to) const;

    /*! \brief Convert horizontal distance to string
     *
     *  This method converts a horizontal distance to a localized string, taking horizontalDistanceUnit into account.
     *
     *  @param distance Distance
     *
     *  @returns A string of the form "280 km", or "-" for an invalid distance
     */
    [[nodiscard]] Q_INVOKABLE QString horizontalDistanceToString(Units::Distance distance) const;

    /*! \brief Convert horizontal speed to string
     *
     *  This method converts a horizontal speed to a localized string, taking horizontalDistanceUnit into account.
     *
     *  @param speed Speed
     *
     *  @returns A string of the form "98 kn", "154 km/h", or "-"
     */
    [[nodiscard]] Q_INVOKABLE QString horizontalSpeedToString(Units::Speed speed) const;

    /*! \brief Reads aircraft data from a JSON document
     *
     * This method loads reads data from a JSON document and stores it in the present object. Notifier signals are emitted as
     * appropriate.  If this method returns a non-empty string, then
     * the JSON data might be partially read.
     *
     * @param fileName File name
     *
     * @returns Empty string in case of success, human-readable, translated
     * error message otherwise.
     */
    [[nodiscard]] Q_INVOKABLE QString loadFromJSON(const QString& fileName);

    /*! \brief Reads aircraft data from a JSON document
     *
     * This method loads reads data from a JSON document and stores it in the present object. Notifier signals are emitted as
     * appropriate.  If this method returns a non-empty string, then
     * the JSON data might be partially read.
     *
     * @param JSON JSON data
     *
     * @returns Empty string in case of success, human-readable, translated
     * error message otherwise.
     */
    [[nodiscard]] Q_INVOKABLE QString loadFromJSON(const QByteArray &JSON);

    /*! \brief Equality check
     *
     *  @param other Aircraft that is compared to this
     *
     *  @result equality
     */
    [[nodiscard]] Q_INVOKABLE bool operator==(const Navigation::Aircraft& other) const = default;

    /*! \brief Saves aircraft to a file
     *
     * This method saves the aircraft as a JSON file.
     *
     * @param fileName File name, needs to include path and extension
     *
     * @returns Empty string in case of success, human-readable, translated
     * error message otherwise.
     */
    [[nodiscard]] Q_INVOKABLE QString save(const QString& fileName) const;

    /*! \brief Exports to route to JSON
     *
     * This method serialises the object as a JSON
     * document.
     *
     * @returns QByteArray containing JSON code
     */
    [[nodiscard]] Q_INVOKABLE QByteArray toJSON() const;

    /*! \brief Convert vertical distance to string
     *
     *  This method converts a vertical distance to a localized string, taking verticalDistanceUnit into account.
     *
     *  @param distance Distance
     *
     *  @param forceSign Prepend positive number with a sign "+"
     *
     *  @returns A string of the form "1.280 m", "3.500 ft", or "-" for an invalid distance
     */
    [[nodiscard]] Q_INVOKABLE QString verticalDistanceToString(Units::Distance distance, bool forceSign=false) const;

    /*! \brief Convert vertical speed to string
     *
     *  This method converts a vertical speed to a localized string, taking verticalDistanceUnit into account.
     *
     *  @param speed Speed
     *
     *  @returns A string of the form "500 ft/min", "2,5 m/s", or "-"
     */
    [[nodiscard]] Q_INVOKABLE QString verticalSpeedToString(Units::Speed speed) const;

    /*! \brief Convert volume to string
     *
     *  This method converts a volume to a localized string, taking volumeUnit into account.
     *
     *  @param volume Volume
     *
     *  @returns A string of the form "5.2 l", or "-"
     */
    [[nodiscard]] Q_INVOKABLE QString volumeToString(Units::Volume volume) const;

private:
    static constexpr Units::Speed minValidSpeed = Units::Speed::fromKN(10.0);
    static constexpr Units::Speed maxValidSpeed = Units::Speed::fromKN(400.0);
    static constexpr Units::Speed minValidVerticalSpeed = Units::Speed::fromFPM(-3000.0);
    static constexpr Units::Speed maxValidVerticalSpeed = Units::Speed::fromFPM(3000.0);
    static constexpr Units::VolumeFlow minValidFuelConsumption = Units::VolumeFlow::fromLPH(0.0);
    static constexpr Units::VolumeFlow maxValidFuelConsumption = Units::VolumeFlow::fromLPH(300.0);
    static constexpr Units::Volume maxValidFuelVolume = Units::Volume::fromL(10000.0);

    bool m_cabinPressureEqualsStaticPressure {false};
    Units::Volume m_maxFuelCapacity {};
    Units::Volume m_unusableFuel {};
    Units::Volume m_additionalFuel {};
    Units::Speed m_climbSpeed {};
    Units::Speed m_climbVerticalSpeed {};
    Units::VolumeFlow m_climbFuelConsumption {};
    Units::Speed m_cruiseSpeed {};
    Units::Speed m_cruiseVerticalSpeed {};
    Units::Speed m_descentSpeed {};
    Units::Speed m_descentVerticalSpeed {};
    Units::VolumeFlow m_descentFuelConsumption {};
    Units::VolumeFlow m_fuelConsumption {};
    FuelConsumptionUnit m_fuelConsumptionUnit {LiterPerHour};
    Units::Speed m_glideSpeed {};
    Units::Speed m_glideVerticalSpeed {};
    HorizontalDistanceUnit m_horizontalDistanceUnit {NauticalMile};
    QString m_icaoType;
    Units::Speed m_minimumSpeed {};
    QString m_name;
    QString m_transponderCode;
    VerticalDistanceUnit m_verticalDistanceUnit{Feet};
    WBWeightUnit m_wbWeightUnit {WBKilogram};
    WBLengthUnit m_wbLengthUnit {WBMillimeter};
    double m_wbEmptyMass = qQNaN();
    double m_wbEmptyArm = qQNaN();
    double m_wbPilotFrontMaxMass = qQNaN();
    double m_wbPilotFrontArm = qQNaN();
    double m_wbRearPaxMaxMass = qQNaN();
    double m_wbRearPaxArm = qQNaN();
    double m_wbCargoMaxMass = qQNaN();
    double m_wbCargoArm = qQNaN();
    double m_wbFuelMaxMass = qQNaN();
    double m_wbFuelArm = qQNaN();
    double m_wbOilMaxMass = qQNaN();
    double m_wbOilArm = qQNaN();
    QVariantList m_wbEnvMasses {};
    QVariantList m_wbEnvArms {};
};

} // namespace Navigation

// Make enums available in QML
namespace AircraftQML {
Q_NAMESPACE
QML_FOREIGN_NAMESPACE(Navigation::Aircraft)
QML_NAMED_ELEMENT(Aircraft)
} // Namespace AircraftQML

// Declare meta types
Q_DECLARE_METATYPE(Navigation::Aircraft)
