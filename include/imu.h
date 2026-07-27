/*
Handles IMU interaction stuff directly with declarations to make stuff public
*/

#pragma once

#include <cstdint>

#include "SparkFunLSM6DSO.h"
#include "sensor_universal.h"

#define LSM6DSO_IMU_ADDRESS 0x6A   // can be 0x6A with a specific hardware setup

class Imu 
{
    public:
        // One-shot raw register readout for logging, calibration, and sanity checks.
        struct RawSample
        {
            std::int16_t accelX;
            std::int16_t accelY;
            std::int16_t accelZ;
            std::int16_t gyroX;
            std::int16_t gyroY;
            std::int16_t gyroZ;
            std::int16_t temperature;
        };

        // Generic 3-axis container used for acceleration and angular-rate vectors.
        struct Vector3
        {
            float x;
            float y;
            float z;
        };

        // Coherent converted sample for flight-control loops and telemetry packets.
        struct MotionSample
        {
            Vector3 accelG;
            Vector3 gyroDps;
            float temperatureC;
            float accelMagnitudeG;
            float gyroMagnitudeDps;
        };

    private:
        // Sensor
        LSM6DSO imu;

        // Meta
        SensorStatus status;
        std::uint8_t consecutiveFailures;                      // helper to count how many status checks failed
        std::uint8_t consecutiveSuccesses;                     // helper to count how many status checks succeeded
        static constexpr std::uint8_t FAILURE_THRESHOLD = 4;   // 40ms at 100Hz
        static constexpr std::uint8_t RECOVERY_THRESHOLD = 9; // 90ms at 100Hz
        static constexpr std::uint8_t FULL_RECOVERY_THRESHOLD = 15; // 150ms at 100Hz
        static constexpr std::uint8_t DEGRADE_THRESHOLD = 2;   // 20ms at 100Hz

    public:
        // Creates the driver wrapper; call begin() before reading data.
        Imu();

        // Starts the LSM6DSO on the configured I2C bus and address.
        bool begin();

        // Updates the health state using an I2C ACK check and debounce thresholds.
        SensorStatus checkHealth();

        // Returns the last health state calculated by begin() or checkHealth().
        SensorStatus getStatus() const;

        // Raw accelerometer ADC counts from the sensor registers.
        std::int16_t getRawAccelX();
        std::int16_t getRawAccelY();
        std::int16_t getRawAccelZ();

        // Raw gyroscope ADC counts from the sensor registers.
        std::int16_t getRawGyroX();
        std::int16_t getRawGyroY();
        std::int16_t getRawGyroZ();

        // Raw temperature register value, useful for debugging conversion behavior.
        std::int16_t getRawTemperature();

        // Reads all raw accelerometer, gyroscope, and temperature channels.
        RawSample getRawSample();

        // Converted acceleration in g for each axis.
        float getAccelXG();
        float getAccelYG();
        float getAccelZG();

        // Converted acceleration vector in g.
        Vector3 getAccelG();

        // Vector magnitude of acceleration in g; useful for launch and impact events.
        float getAccelMagnitudeG();

        // Converted angular rate in degrees per second for each axis.
        float getGyroXDps();
        float getGyroYDps();
        float getGyroZDps();

        // Converted angular-rate vector in degrees per second.
        Vector3 getGyroDps();

        // Vector magnitude of angular rate in degrees per second; useful for spin detection.
        float getGyroMagnitudeDps();

        // Sensor die temperature in Celsius or Fahrenheit.
        float getTemperatureC();
        float getTemperatureF();

        // Reads converted accel, gyro, temperature, and vector magnitudes together.
        MotionSample getMotionSample();

        // Returns the LSM6DSO data-ready bit flags.
        std::uint8_t getDataReadyFlags();

        // Convenience checks for whether fresh accel, gyro, or temp data is available.
        bool isAccelDataReady();
        bool isGyroDataReady();
        bool isTemperatureDataReady();

        // Configures broad-range, high-performance sampling defaults for rockets/drones.
        bool configureForFlight(std::uint8_t accelRangeG = 16, std::uint16_t gyroRangeDps = 2000, std::uint16_t sampleRateHz = 416);

        // Detects readings close to configured sensor limits so saturation can be flagged.
        bool isAccelNearLimit(float marginG = 0.5F);
        bool isGyroNearLimit(float marginDps = 50.0F);

};
