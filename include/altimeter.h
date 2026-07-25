/*
altimeter stuff
*/

#pragma once
#include "sensor_universal.h"

#define BMP280_ADDRESS 0x76 // When SDO connected to GND, 0x77 when connected to V_DDIO - MUST BE CONNECTED TO SOMETHING

class Altimeter
{
    private:
        SensorStatus status;
        Adafruit_BMP280 bmp;
        uint8_t consecutiveFailures;    // helper to count how many status checks failed
        uint8_t consecutiveSuccesses;   // helper to count how many status checks succeeded
        static const uint8_t FAILURE_THRESHOLD = 4;     // 40ms at 100Hz
        static const uint8_t RECOVERY_THRESHOLD = 18;    // 180ms at 100Hz

    public:
        // constructor declaration
        Altimeter();

        // Method declarations
        bool begin();
        SensorStatus checkHealth();
        SensorStatus getStatus() const; 

        // getters
        float getTemperature();
        float getPressure();
        float getAltitude(float seaLevelhPa = (1013.25F));
};