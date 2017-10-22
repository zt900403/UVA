#include "uav.h"

UAV::UAV()
{

}

float UAV::acceleration() const
{
    return m_acceleration;
}

void UAV::setAcceleration(float acceleration)
{
    m_acceleration = acceleration;
}

QString UAV::description() const
{
    return m_description;
}

void UAV::setDescription(const QString &description)
{
    m_description = description;
}

float UAV::flightHeight() const
{
    return m_flightHeight;
}

void UAV::setFlightHeight(float flightHeight)
{
    m_flightHeight = flightHeight;
}

float UAV::flyEndurance() const
{
    return m_flyEndurance;
}

void UAV::setFlyEndurance(float flyEndurance)
{
    m_flyEndurance = flyEndurance;
}

QString UAV::imageFile() const
{
    return m_imageFile;
}

void UAV::setImageFile(const QString &imageFile)
{
    m_imageFile = imageFile;
}

float UAV::loadWeight() const
{
    return m_loadWeight;
}

void UAV::setLoadWeight(float loadWeight)
{
    m_loadWeight = loadWeight;
}

float UAV::maxSpeed() const
{
    return m_maxSpeed;
}

void UAV::setMaxSpeed(float maxSpeed)
{
    m_maxSpeed = maxSpeed;
}

QString UAV::name() const
{
    return m_name;
}

void UAV::setName(const QString &name)
{
    m_name = name;
}

float UAV::voyage() const
{
    return m_voyage;
}

void UAV::setVoyage(float voyage)
{
    m_voyage = voyage;
}

float UAV::weight() const
{
    return m_weight;
}

void UAV::setWeight(float weight)
{
    m_weight = weight;
}

QMap<QString, int> UAV::weapon() const
{
    return m_weapon;
}

void UAV::setWeapon(const QMap<QString, int> &weapon)
{
    m_weapon = weapon;
}

QBitmap UAV::bitmap() const
{
    return m_bitmap;
}

void UAV::setBitmap(const QBitmap &bitmap)
{
    m_bitmap = bitmap;
}