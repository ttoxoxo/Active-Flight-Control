#include <Wire.h>
#include <SPI.h>
#include <altimeter.h>

// Constructor
Altimeter::Altimeter()
    : bmp(&Wire2), status(SensorStatus::UNINITIALIZED), consecutiveSuccesses(0), consecutiveFailures(0)
{
}

// Altimeter meta functioning
bool Altimeter::begin()
{
    bool ok = bmp.begin(ALTIMETER_BMP280_ADDRESS);
    status = ok ? SensorStatus::NOMINAL : SensorStatus::FAILED;
    return ok;
}

SensorStatus Altimeter::checkHealth()
{
    Wire2.beginTransmission(ALTIMETER_BMP280_ADDRESS);
    bool ack = (Wire2.endTransmission() == 0);

    if (ack)
    {
        consecutiveFailures = 0;
        consecutiveSuccesses++;
        if (consecutiveSuccesses >= FULL_RECOVERY_THRESHOLD)
            status = SensorStatus::NOMINAL;
        else if (consecutiveSuccesses >= RECOVERY_THRESHOLD)
            status = SensorStatus::DEGRADED;
    }
    else
    {
        consecutiveSuccesses = 0;
        consecutiveFailures++;
        if (status != SensorStatus::FAILED && consecutiveFailures >= DEGRADE_THRESHOLD)
            status = SensorStatus::DEGRADED;
        if (consecutiveFailures >= FAILURE_THRESHOLD)
            status = SensorStatus::FAILED;
    }

    return status;
}

SensorStatus Altimeter::getStatus() const
{
    return status;
}


// altimeter data getters

float Altimeter::getTemperature()
{
    if (status == SensorStatus::FAILED)
        return NAN;
    return bmp.readTemperature();
}

float Altimeter::getPressure()
{
    if (status == SensorStatus::FAILED)
        return NAN;
    return bmp.readPressure();
}

float Altimeter::getAltitude(float seaLevelhPa)
{
    if (status == SensorStatus::FAILED)
        return NAN;
    return bmp.readAltitude(seaLevelhPa);
}