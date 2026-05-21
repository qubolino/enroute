/***************************************************************************
 *   Copyright (C) 2019-2024 by Stefan Kebekus                             *
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

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

#include "Aircraft.h"
#include "GlobalObject.h"
#include "Librarian.h"

using namespace Qt::Literals::StringLiterals;



//
// Setter Methods
//

void Navigation::Aircraft::setMaxFuelCapacity(Units::Volume newVolume)
{
    if (newVolume.toL() < 0.0 || newVolume > maxValidFuelVolume) {
        newVolume = Units::Volume();
    }
    m_maxFuelCapacity = newVolume;
}


void Navigation::Aircraft::setUnusableFuel(Units::Volume newVolume)
{
    if (newVolume.toL() < 0.0 || newVolume > maxValidFuelVolume) {
        newVolume = Units::Volume();
    }
    m_unusableFuel = newVolume;
}


void Navigation::Aircraft::setAdditionalFuel(Units::Volume newVolume)
{
    if (newVolume.toL() < 0.0 || newVolume > maxValidFuelVolume) {
        newVolume = Units::Volume();
    }
    m_additionalFuel = newVolume;
}


void Navigation::Aircraft::setClimbSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidSpeed) || (newSpeed > maxValidSpeed)) {
        newSpeed = Units::Speed();
    }
    m_climbSpeed = newSpeed;
}


void Navigation::Aircraft::setClimbVerticalSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidVerticalSpeed) || (newSpeed > maxValidVerticalSpeed)) {
        newSpeed = Units::Speed();
    }
    m_climbVerticalSpeed = newSpeed;
}


void Navigation::Aircraft::setClimbFuelConsumption(Units::VolumeFlow newFlow)
{
    if ((newFlow < minValidFuelConsumption) || (newFlow > maxValidFuelConsumption)) {
        newFlow = Units::VolumeFlow();
    }
    m_climbFuelConsumption = newFlow;
}


void Navigation::Aircraft::setCruiseSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidSpeed) || (newSpeed > maxValidSpeed)) {
        newSpeed = Units::Speed();
    }
    m_cruiseSpeed = newSpeed;
}


void Navigation::Aircraft::setCruiseVerticalSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidVerticalSpeed) || (newSpeed > maxValidVerticalSpeed)) {
        newSpeed = Units::Speed();
    }
    m_cruiseVerticalSpeed = newSpeed;
}


void Navigation::Aircraft::setDescentSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidSpeed) || (newSpeed > maxValidSpeed)) {
        newSpeed = Units::Speed();
    }
    m_descentSpeed = newSpeed;
}


void Navigation::Aircraft::setDescentVerticalSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidVerticalSpeed) || (newSpeed > maxValidVerticalSpeed)) {
        newSpeed = Units::Speed();
    }
    m_descentVerticalSpeed = newSpeed;
}


void Navigation::Aircraft::setDescentFuelConsumption(Units::VolumeFlow newFlow)
{
    if ((newFlow < minValidFuelConsumption) || (newFlow > maxValidFuelConsumption)) {
        newFlow = Units::VolumeFlow();
    }
    m_descentFuelConsumption = newFlow;
}


void Navigation::Aircraft::setGlideSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidSpeed) || (newSpeed > maxValidSpeed)) {
        newSpeed = Units::Speed();
    }
    m_glideSpeed = newSpeed;
}


void Navigation::Aircraft::setGlideVerticalSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidVerticalSpeed) || (newSpeed > maxValidVerticalSpeed)) {
        newSpeed = Units::Speed();
    }
    m_glideVerticalSpeed = newSpeed;
}


void Navigation::Aircraft::setFuelConsumption(Units::VolumeFlow newFuelConsumption) {
    if ((newFuelConsumption < minValidFuelConsumption) || (newFuelConsumption > maxValidFuelConsumption)) {
        newFuelConsumption = Units::VolumeFlow();
    }

    if (!qFuzzyCompare(newFuelConsumption.toLPH(), m_fuelConsumption.toLPH())) {
        m_fuelConsumption = newFuelConsumption;
    }
}


void Navigation::Aircraft::setFuelConsumptionUnit(FuelConsumptionUnit newUnit)
{
    m_fuelConsumptionUnit = newUnit;
}


void Navigation::Aircraft::setHorizontalDistanceUnit(HorizontalDistanceUnit newUnit)
{
    m_horizontalDistanceUnit = newUnit;
}


void Navigation::Aircraft::setMinimumSpeed(Units::Speed newSpeed)
{
    if ((newSpeed < minValidSpeed) || (newSpeed > maxValidSpeed)) {
        newSpeed = Units::Speed();
    }
    m_minimumSpeed = newSpeed;
}


void Navigation::Aircraft::setIcaoType(const QString& newIcaoType)
{
    m_icaoType = newIcaoType;
}


void Navigation::Aircraft::setName(const QString& newName)
{
    m_name = newName;
}


void Navigation::Aircraft::setTransponderCode(const QString& newCode)
{
    m_transponderCode = newCode;
}


void Navigation::Aircraft::setVerticalDistanceUnit(VerticalDistanceUnit newUnit)
{
    m_verticalDistanceUnit = newUnit;
}


void Navigation::Aircraft::setWbWeightUnit(WBWeightUnit u) { m_wbWeightUnit = u; }
void Navigation::Aircraft::setWbLengthUnit(WBLengthUnit u) { m_wbLengthUnit = u; }
void Navigation::Aircraft::setWbEmptyMass(double v) { m_wbEmptyMass = v; }
void Navigation::Aircraft::setWbEmptyArm(double v) { m_wbEmptyArm = v; }
void Navigation::Aircraft::setWbPilotFrontMaxMass(double v) { m_wbPilotFrontMaxMass = v; }
void Navigation::Aircraft::setWbPilotFrontArm(double v) { m_wbPilotFrontArm = v; }
void Navigation::Aircraft::setWbRearPaxMaxMass(double v) { m_wbRearPaxMaxMass = v; }
void Navigation::Aircraft::setWbRearPaxArm(double v) { m_wbRearPaxArm = v; }
void Navigation::Aircraft::setWbCargoMaxMass(double v) { m_wbCargoMaxMass = v; }
void Navigation::Aircraft::setWbCargoArm(double v) { m_wbCargoArm = v; }
void Navigation::Aircraft::setWbFuelMaxMass(double v) { m_wbFuelMaxMass = v; }
void Navigation::Aircraft::setWbFuelArm(double v) { m_wbFuelArm = v; }
void Navigation::Aircraft::setWbOilMaxMass(double v) { m_wbOilMaxMass = v; }
void Navigation::Aircraft::setWbOilArm(double v) { m_wbOilArm = v; }

void Navigation::Aircraft::setWbEnvMasses(const QVariantList& v)
{
    m_wbEnvMasses = v.mid(0, 7);
}

void Navigation::Aircraft::setWbEnvArms(const QVariantList& v)
{
    m_wbEnvArms = v.mid(0, 7);
}


//
// Methods
//

QString Navigation::Aircraft::describeWay(const QGeoCoordinate &from, const QGeoCoordinate &to) const
{
    // Paranoid safety checks
    if (!from.isValid())
    {
        return {};
    }
    if (!to.isValid())
    {
        return {};
    }

    QStringList result;

    auto dist = Units::Distance::fromM(from.distanceTo(to));
    result += QStringLiteral("DIST %1").arg(horizontalDistanceToString(dist));

    auto QUJ = qRound(from.azimuthTo(to));
    result += QStringLiteral("QUJ %1°").arg(QUJ);

    if ((from.type() == QGeoCoordinate::Coordinate3D) && (to.type() == QGeoCoordinate::Coordinate3D))
    {
        auto hDistM = from.distanceTo(to);
        auto vDistM = from.altitude() - to.altitude() - 300;

        if (!qIsNaN(hDistM) && !qIsNaN(vDistM) &&(hDistM > 1000.0))
        {
            if (vDistM < 1)
            {
                result += QStringLiteral("E ∞");
            }
            else
            {
                auto E = hDistM/vDistM;
                if (!qIsNaN(E) && (E > 100))
                {
                    result += QStringLiteral("E ∞");
                }
                else
                {
                    result += QStringLiteral("E %1").arg(qRound(E));
                }
            }
        }

    }

    return result.join(u" • "_s);
}


QString Navigation::Aircraft::horizontalDistanceToString(Units::Distance distance) const
{
    if (!distance.isFinite()) {
        return QStringLiteral("-");
    }

    switch(m_horizontalDistanceUnit) {
    case Navigation::Aircraft::NauticalMile:
        if (distance > Units::Distance::fromNM(10.0)) {
            return QStringLiteral("%L1 nm").arg(distance.toNM(), 0, 'f', 0);
        }
        return QStringLiteral("%L1 nm").arg(distance.toNM(), 0, 'f', 1);
    case Navigation::Aircraft::Kilometer:
        if (distance > Units::Distance::fromKM(10.0)) {
            return QStringLiteral("%L1 km").arg(distance.toKM(), 0, 'f', 0);
        }
        return QStringLiteral("%L1 km").arg(distance.toKM(), 0, 'f', 1);
    case Navigation::Aircraft::StatuteMile:
        if (distance > Units::Distance::fromMIL(10.0)) {
            return QStringLiteral("%L1 mil").arg(distance.toMIL(), 0, 'f', 0);
        }
        return QStringLiteral("%L1 mil").arg(distance.toMIL(), 0, 'f', 1);
    }
    return {};
}


QString Navigation::Aircraft::horizontalSpeedToString(Units::Speed speed) const
{
    if (!speed.isFinite())
    {
        return QStringLiteral("-");
    }

    switch(m_horizontalDistanceUnit)
    {
    case Navigation::Aircraft::NauticalMile:
        return QStringLiteral("%L1 kn").arg(qRound( speed.toKN() ));
    case Navigation::Aircraft::Kilometer:
        return QStringLiteral("%L1 km/h").arg(qRound( speed.toKMH() ));
    case Navigation::Aircraft::StatuteMile:
        return QStringLiteral("%L1 mph").arg(qRound( speed.toMPH() ));
    }
    return {};
}


QString Navigation::Aircraft::loadFromJSON(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return QObject::tr("Unable to open the file '%1' for reading.").arg(fileName);
    }
    return loadFromJSON(file.readAll());
}


QString Navigation::Aircraft::loadFromJSON(const QByteArray &JSON)
{
    QJsonParseError parseError{};
    auto document = QJsonDocument::fromJson(JSON, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return parseError.errorString();
    }

    auto content = document.object();
    if (content.isEmpty()) {
        return QObject::tr("JSON document contains no data.");
    }

    if (content[QStringLiteral("content")] != "aircraft") {
        return QObject::tr("JSON document does not describe an aircraft.");
    }

    setCabinPressureEqualsStaticPressure( content[QStringLiteral("cabinPressureEqualsStaticPressure")].toBool(false) );
    setMaxFuelCapacity( Units::Volume::fromL( content[QStringLiteral("maxFuelCapacity_l")].toDouble(NAN) ));
    setUnusableFuel( Units::Volume::fromL( content[QStringLiteral("unusableFuel_l")].toDouble(NAN) ));
    setAdditionalFuel( Units::Volume::fromL( content[QStringLiteral("additionalFuel_l")].toDouble(NAN) ));
    setClimbSpeed( Units::Speed::fromMPS( content[QStringLiteral("climbSpeed_mps")].toDouble(NAN) ));
    setClimbVerticalSpeed( Units::Speed::fromFPM( content[QStringLiteral("climbVerticalSpeed_fpm")].toDouble(NAN) ));
    setClimbFuelConsumption( Units::VolumeFlow::fromLPH( content[QStringLiteral("climbFuelConsumption_lph")].toDouble(NAN) ));
    setCruiseSpeed( Units::Speed::fromMPS( content[QStringLiteral("cruiseSpeed_mps")].toDouble(NAN) ));
    setCruiseVerticalSpeed( Units::Speed::fromFPM( content[QStringLiteral("cruiseVerticalSpeed_fpm")].toDouble(NAN) ));
    setDescentSpeed( Units::Speed::fromMPS( content[QStringLiteral("descentSpeed_mps")].toDouble(NAN) ));
    setDescentVerticalSpeed( Units::Speed::fromFPM( content[QStringLiteral("descentVerticalSpeed_fpm")].toDouble(NAN) ));
    setDescentFuelConsumption( Units::VolumeFlow::fromLPH( content[QStringLiteral("descentFuelConsumption_lph")].toDouble(NAN) ));
    setFuelConsumption( Units::VolumeFlow::fromLPH( content[QStringLiteral("fuelConsumption_lph")].toDouble(NAN) ));
    setFuelConsumptionUnit( static_cast<FuelConsumptionUnit>(content[QStringLiteral("fuelConsumptionUnit")].toInt(LiterPerHour)) );
    setGlideSpeed( Units::Speed::fromMPS( content[QStringLiteral("glideSpeed_mps")].toDouble(NAN) ));
    setGlideVerticalSpeed( Units::Speed::fromFPM( content[QStringLiteral("glideVerticalSpeed_fpm")].toDouble(NAN) ));
    setHorizontalDistanceUnit( static_cast<HorizontalDistanceUnit>(content[QStringLiteral("horizontalDistanceUnit")].toInt(NauticalMile)) );
    setIcaoType( content[QStringLiteral("icaoType")].toString() );
    setMinimumSpeed( Units::Speed::fromMPS( content[QStringLiteral("minimumSpeed_mps")].toDouble(NAN) ));
    setName( content[QStringLiteral("name")].toString() );
    setTransponderCode( content[QStringLiteral("transponderCode")].toString() );
    setVerticalDistanceUnit( static_cast<VerticalDistanceUnit>(content[QStringLiteral("verticalDistanceUnit")].toInt(Feet)) );
    setWbWeightUnit( static_cast<WBWeightUnit>(content[QStringLiteral("wbWeightUnit")].toInt(WBKilogram)) );
    setWbLengthUnit( static_cast<WBLengthUnit>(content[QStringLiteral("wbLengthUnit")].toInt(WBMillimeter)) );
    setWbEmptyMass( content[QStringLiteral("wbEmptyMass_kg")].toDouble(qQNaN()) );
    setWbEmptyArm( content[QStringLiteral("wbEmptyArm_m")].toDouble(qQNaN()) );
    setWbPilotFrontMaxMass( content[QStringLiteral("wbPilotFrontMaxMass_kg")].toDouble(qQNaN()) );
    setWbPilotFrontArm( content[QStringLiteral("wbPilotFrontArm_m")].toDouble(qQNaN()) );
    setWbRearPaxMaxMass( content[QStringLiteral("wbRearPaxMaxMass_kg")].toDouble(qQNaN()) );
    setWbRearPaxArm( content[QStringLiteral("wbRearPaxArm_m")].toDouble(qQNaN()) );
    setWbCargoMaxMass( content[QStringLiteral("wbCargoMaxMass_kg")].toDouble(qQNaN()) );
    setWbCargoArm( content[QStringLiteral("wbCargoArm_m")].toDouble(qQNaN()) );
    setWbFuelMaxMass( content[QStringLiteral("wbFuelMaxMass_kg")].toDouble(qQNaN()) );
    setWbFuelArm( content[QStringLiteral("wbFuelArm_m")].toDouble(qQNaN()) );
    setWbOilMaxMass( content[QStringLiteral("wbOilMaxMass_kg")].toDouble(qQNaN()) );
    setWbOilArm( content[QStringLiteral("wbOilArm_m")].toDouble(qQNaN()) );
    {
        QVariantList masses, arms;
        const auto massArr = content[QStringLiteral("wbEnvMasses_kg")].toArray();
        const auto armArr  = content[QStringLiteral("wbEnvArms_m")].toArray();
        for (const auto& v : massArr) masses.append(v.toDouble(qQNaN()));
        for (const auto& v : armArr)  arms.append(v.toDouble(qQNaN()));
        setWbEnvMasses(masses);
        setWbEnvArms(arms);
    }

    return {};
}


QString Navigation::Aircraft::save(const QString& fileName) const
{
    // Make directory, if it does not yet exist.
    QDir const dir;
    dir.mkpath(Librarian::directory(Librarian::Aircraft));

    QFile file(fileName);
    auto success = file.open(QIODevice::WriteOnly);
    if (!success) {
        return QObject::tr("Unable to open the file '%1' for writing.").arg(fileName);
    }
    auto numBytesWritten = file.write(toJSON());
    if (numBytesWritten == -1) {
        file.close();
        QFile::remove(fileName);
        return QObject::tr("Unable to write to file '%1'.").arg(fileName);
    }
    return {};
}


QByteArray Navigation::Aircraft::toJSON() const
{
    QJsonObject jsonObj;
    jsonObj.insert(QStringLiteral("content"), "aircraft");

    jsonObj.insert(QStringLiteral("cabinPressureEqualsStaticPressure"), m_cabinPressureEqualsStaticPressure);
    jsonObj.insert(QStringLiteral("maxFuelCapacity_l"), m_maxFuelCapacity.toL());
    jsonObj.insert(QStringLiteral("unusableFuel_l"), m_unusableFuel.toL());
    jsonObj.insert(QStringLiteral("additionalFuel_l"), m_additionalFuel.toL());
    jsonObj.insert(QStringLiteral("climbSpeed_mps"), m_climbSpeed.toMPS());
    jsonObj.insert(QStringLiteral("climbVerticalSpeed_fpm"), m_climbVerticalSpeed.toFPM());
    jsonObj.insert(QStringLiteral("climbFuelConsumption_lph"), m_climbFuelConsumption.toLPH());
    jsonObj.insert(QStringLiteral("cruiseSpeed_mps"), m_cruiseSpeed.toMPS());
    jsonObj.insert(QStringLiteral("cruiseVerticalSpeed_fpm"), m_cruiseVerticalSpeed.toFPM());
    jsonObj.insert(QStringLiteral("descentSpeed_mps"), m_descentSpeed.toMPS());
    jsonObj.insert(QStringLiteral("descentVerticalSpeed_fpm"), m_descentVerticalSpeed.toFPM());
    jsonObj.insert(QStringLiteral("descentFuelConsumption_lph"), m_descentFuelConsumption.toLPH());
    jsonObj.insert(QStringLiteral("fuelConsumption_lph"), m_fuelConsumption.toLPH());
    jsonObj.insert(QStringLiteral("fuelConsumptionUnit"), m_fuelConsumptionUnit);
    jsonObj.insert(QStringLiteral("glideSpeed_mps"), m_glideSpeed.toMPS());
    jsonObj.insert(QStringLiteral("glideVerticalSpeed_fpm"), m_glideVerticalSpeed.toFPM());
    jsonObj.insert(QStringLiteral("horizontalDistanceUnit"), m_horizontalDistanceUnit);
    jsonObj.insert(QStringLiteral("icaoType"), m_icaoType);
    jsonObj.insert(QStringLiteral("minimumSpeed_mps"), m_minimumSpeed.toMPS());
    jsonObj.insert(QStringLiteral("name"), m_name);
    jsonObj.insert(QStringLiteral("transponderCode"), m_transponderCode);
    jsonObj.insert(QStringLiteral("verticalDistanceUnit"), m_verticalDistanceUnit);
    jsonObj.insert(QStringLiteral("wbWeightUnit"), m_wbWeightUnit);
    jsonObj.insert(QStringLiteral("wbLengthUnit"), m_wbLengthUnit);
    jsonObj.insert(QStringLiteral("wbEmptyMass_kg"), m_wbEmptyMass);
    jsonObj.insert(QStringLiteral("wbEmptyArm_m"), m_wbEmptyArm);
    jsonObj.insert(QStringLiteral("wbPilotFrontMaxMass_kg"), m_wbPilotFrontMaxMass);
    jsonObj.insert(QStringLiteral("wbPilotFrontArm_m"), m_wbPilotFrontArm);
    jsonObj.insert(QStringLiteral("wbRearPaxMaxMass_kg"), m_wbRearPaxMaxMass);
    jsonObj.insert(QStringLiteral("wbRearPaxArm_m"), m_wbRearPaxArm);
    jsonObj.insert(QStringLiteral("wbCargoMaxMass_kg"), m_wbCargoMaxMass);
    jsonObj.insert(QStringLiteral("wbCargoArm_m"), m_wbCargoArm);
    jsonObj.insert(QStringLiteral("wbFuelMaxMass_kg"), m_wbFuelMaxMass);
    jsonObj.insert(QStringLiteral("wbFuelArm_m"), m_wbFuelArm);
    jsonObj.insert(QStringLiteral("wbOilMaxMass_kg"), m_wbOilMaxMass);
    jsonObj.insert(QStringLiteral("wbOilArm_m"), m_wbOilArm);
    {
        QJsonArray massArr, armArr;
        for (const auto& v : m_wbEnvMasses) massArr.append(v.toDouble());
        for (const auto& v : m_wbEnvArms)  armArr.append(v.toDouble());
        jsonObj.insert(QStringLiteral("wbEnvMasses_kg"), massArr);
        jsonObj.insert(QStringLiteral("wbEnvArms_m"), armArr);
    }

    QJsonDocument doc;
    doc.setObject(jsonObj);
    return doc.toJson();
}


QString Navigation::Aircraft::verticalDistanceToString(Units::Distance distance, bool forceSign) const
{
    if (!distance.isFinite())
    {
        return QStringLiteral("-");
    }
    QString signString;
    if (forceSign && (distance.toM() >= 0.0))
    {
        signString = QStringLiteral("+");
    }
    switch(m_verticalDistanceUnit)
    {
    case Navigation::Aircraft::Feet:
        return signString+QStringLiteral("%L1 ft").arg(qRound(distance.toFeet()));
    case Navigation::Aircraft::Meters:
        return signString+QStringLiteral("%L1 m").arg(qRound(distance.toM()));
    }
    return {};
}


QString Navigation::Aircraft::verticalSpeedToString(Units::Speed speed) const
{
    if (!speed.isFinite())
    {
        return QStringLiteral("-");
    }
    switch(m_verticalDistanceUnit)
    {
    case Navigation::Aircraft::Feet:
        return QStringLiteral("%L1 ft/min").arg(qRound( speed.toFPM() ));
    case Navigation::Aircraft::Meters:
        return QStringLiteral("%L1 m/s").arg(qRound( speed.toMPS() ));
    }
    return {};
}


QString Navigation::Aircraft::volumeToString(Units::Volume volume) const
{
    if (!volume.isFinite())
    {
        return QStringLiteral("-");
    }
    switch(m_fuelConsumptionUnit)
    {
    case Navigation::Aircraft::LiterPerHour:
        if (volume.toL() < 10.0)
        {
            return QStringLiteral("%L1 l").arg(volume.toL(), 0, 'f', 1);
        }
        return QStringLiteral("%L1 l").arg(qRound( volume.toL() ));
    case Navigation::Aircraft::GallonPerHour:
        if (volume.toGAL() < 10.0)
        {
            return QStringLiteral("%L1 gal").arg(volume.toGAL(), 0, 'f', 1);
        }
        return QStringLiteral("%L1 gal").arg(qRound( volume.toGAL() ));
    }
    return {};
}
