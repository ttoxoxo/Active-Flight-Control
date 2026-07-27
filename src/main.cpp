#include <Arduino.h>
#include <cmath>

#include "config.h"
#include "altimeter.h"
#include "imu.h"

Altimeter alti = Altimeter();
Imu imu = Imu();
float seaLevelPressurehPa;
float seaLevelPressurePa;

void setup()
{
    Serial.begin(DEBUG_BAUD);

    while (!Serial && millis() < 4000)
    {
        // Give the USB serial monitor a moment to attach after upload/reset.
    }

    Serial.println("Entered Setup!");

    // Activate I2C lines - some libraries do it, but preemptive activation ensures that they work
    Wire.begin(); 
    Wire1.begin();
    Wire2.begin();

    // Altimeter
    Serial.println(alti.begin() ? "Detected and Started" : "Failed");

    // Assumed to be on ground during setup
    Serial.println("\nAltimeter Pressure Reading:");
    seaLevelPressurePa = alti.getPressure();
    seaLevelPressurehPa = seaLevelPressurePa / 100;
    Serial.println(seaLevelPressurehPa);

    // IMU
    Serial.println("\nIMU:");
    Serial.println(imu.begin() ? "Detected and Started" : "Failed");
    Serial.println(imu.configureForFlight() ? "Configured for flight" : "Configuration failed");
    
    Serial.println("SETUP ENDED\n---\n");
}

void loop()
{

    Serial.println("Altimeter Health Check:");
    printStatus(alti.checkHealth());

    Serial.println("\nAltimeter Temperature Reading:");
    Serial.println(alti.getTemperature());

    Serial.println("\nAltimeter Altitude Reading:");
    Serial.println(alti.getAltitude(seaLevelPressurehPa));

    Serial.println("\n---\n");

    // ---- IMU ----
    
    Serial.println("IMU Health Check:");
    printStatus(imu.checkHealth());
    
    Imu::RawSample raw = imu.getRawSample();
    Serial.println("\nIMU Raw Sample:");
    Serial.print("  accel raw: ");
    Serial.print(raw.accelX);
    Serial.print(", ");
    Serial.print(raw.accelY);
    Serial.print(", ");
    Serial.println(raw.accelZ);
    Serial.print("  gyro raw:  ");
    Serial.print(raw.gyroX);
    Serial.print(", ");
    Serial.print(raw.gyroY);
    Serial.print(", ");
    Serial.println(raw.gyroZ);
    Serial.print("  temp raw:  ");
    Serial.println(raw.temperature);

    Imu::MotionSample motion = imu.getMotionSample();
    Serial.println("\nIMU Motion Sample:");
    Serial.print("  accel (g):   x=");
    Serial.print(motion.accelG.x, 3);
    Serial.print(" y=");
    Serial.print(motion.accelG.y, 3);
    Serial.print(" z=");
    Serial.println(motion.accelG.z, 3);
    Serial.print("  gyro (dps):  x=");
    Serial.print(motion.gyroDps.x, 3);
    Serial.print(" y=");
    Serial.print(motion.gyroDps.y, 3);
    Serial.print(" z=");
    Serial.println(motion.gyroDps.z, 3);
    Serial.print("  accel mag (g): ");
    Serial.println(motion.accelMagnitudeG, 3);
    Serial.print("  gyro mag (dps): ");
    Serial.println(motion.gyroMagnitudeDps, 3);
    Serial.print("  temp (C): ");
    Serial.println(motion.temperatureC, 2);

    Serial.print("  accel near limit: ");
    Serial.println(imu.isAccelNearLimit() ? "YES" : "no");
    Serial.print("  gyro near limit:  ");
    Serial.println(imu.isGyroNearLimit() ? "YES" : "no");

    Serial.println("\n---\n");
    
    delay(1000);
}