/*
Handles reading directly from the GPS with declarations to make stuff public
*/

#pragma once

#include <cstdint>

#include <SparkFun_u-blox_GNSS_v3.h>


#include "sensor_universal.h"
#include "config.h"

struct GnssData
{
    // Position
    double latitude;   // degrees
    double longitude;  // degrees
    float altitudeMSL; // meters

    // Velocity, NED frame
    float velNorth; // m/s
    float velEast;  // m/s
    float velDown;  // m/s

    // Accuracy estimates
    float horizontalAcc; // meters
    float verticalAcc;   // meters
    float speedAcc;      // m/s

    // Fix quality / metadata
    uint8_t fixType;
    uint8_t satellites;
    float pdop;

    uint32_t timeOfWeekMs;
    bool valid;

    void print() const
    {
        if (valid)
        {
            Serial.println("--- GNSS Data ---");

            // Position (Access members directly)
            Serial.print("Lat: ");
            Serial.println(latitude, 7);
            Serial.print("Lon: ");
            Serial.println(longitude, 7);
            Serial.print("Alt (MSL): ");
            Serial.print(altitudeMSL, 2);
            Serial.println(" m");

            // Velocity (NED)
            Serial.print("Vel N: ");
            Serial.print(velNorth, 2);
            Serial.print(" | E: ");
            Serial.print(velEast, 2);
            Serial.print(" | D: ");
            Serial.print(velDown, 2);
            Serial.println(" m/s");

            // Accuracy
            Serial.print("Acc Horz: ");
            Serial.print(horizontalAcc, 2);
            Serial.print(" m | Vert: ");
            Serial.print(verticalAcc, 2);
            Serial.print(" m | Speed: ");
            Serial.print(speedAcc, 2);
            Serial.println(" m/s");

            // Fix Quality
            Serial.print("Fix: ");
            Serial.print(fixType);
            Serial.print(" | Sats: ");
            Serial.print(satellites);
            Serial.print(" | PDOP: ");
            Serial.println(pdop, 2);

            // Timing
            Serial.print("TimeOfWeek: ");
            Serial.println(timeOfWeekMs);

            Serial.println("-----------------");
        }
        else
        {
            Serial.println("GNSS Data: INVALID");
        }
    }
};

class GNSS {
    private:
        // Sensor
        SFE_UBLOX_GNSS_SERIAL gnss;
        bool gnc_DEBUG;

        // Meta
        SensorStatus status;
        std::uint8_t consecutiveSuccesses;                          // helper to count how many status checks succeeded
        std::uint8_t consecutiveFailures;                           // helper to count how many status checks failed
        static constexpr std::uint8_t FAILURE_THRESHOLD = 25;        // 2.5s at 10Hz
        static constexpr std::uint8_t RECOVERY_THRESHOLD = 3;       // .3s at 10Hz
        static constexpr std::uint8_t DEGRADE_THRESHOLD = 3;        // .3s at 10Hz
        static constexpr std::uint8_t FULL_RECOVERY_THRESHOLD = 5; // .5s at 10Hz

    public:
        // constructor declaration
        GNSS();

        // Method declarations
        bool begin();
        SensorStatus checkHealth();
        SensorStatus getStatus() const;

        // getters
        GnssData getData(GnssData d);
};