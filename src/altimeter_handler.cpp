#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#include <altimeter.h>

// Constructor
Altimeter::Altimeter()
    : status(SensorStatus::UNINITIALIZED), consecutiveFailures(0), consecutiveSuccesses(0)
{
}

// Altimeter meta functioning
bool Altimeter::begin()
{
    bool ok = bmp.begin(BMP280_ADDRESS);
    status = ok ? SensorStatus::NOMINAL : SensorStatus::FAILED;
    return ok;
}

SensorStatus Altimeter::checkHealth()
{
    Wire.beginTransmission(BMP280_ADDRESS);
    bool ack = (Wire.endTransmission() == 0);

    if (ack)
    {
        consecutiveFailures = 0;
        consecutiveSuccesses++;
        if (consecutiveSuccesses >= RECOVERY_THRESHOLD)
            status = SensorStatus::NOMINAL;
    }
    else
    {
        consecutiveSuccesses = 0;
        consecutiveFailures++;
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

float Altimeter::getAltitude(float seaLevelhPa = (1013.25F))
{
    if (status == SensorStatus::FAILED)
        return NAN;
    return bmp.readAltitude(seaLevelhPa);
}