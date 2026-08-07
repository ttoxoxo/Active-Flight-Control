#include <Arduino.h>
#include <cmath>
#include "LittleFS.h"

#include "config.h"
#include "altimeter.h"
#include "imu.h"
#include "gnss.h"

// Globals
LittleFS_QSPI myfs; // for flash unit test
// ADD VERSIONING HERE

// Objects/Structs
Altimeter alti = Altimeter();
Imu imu = Imu();
GNSS gnss = GNSS();
GnssData gnss_data;

// Sensor global data
// // Altimeter
float seaLevelPressurehPa;
float seaLevelPressurePa;
float rawPressurePa;
float pressurehPa;

// General Servicing
// // Timing State for sensors and loops
unsigned long loopCount = 0;

unsigned long lastSensorTick = 0;
unsigned long lastPrintTime = 0;
constexpr unsigned long SENSOR_TICK_MS = 20; // ~50Hz — comfortably faster than GNSS's 10Hz output,
                                             // fast enough to drain GNSS_SERIAL before it backs up, slow enough that threshold tuning stays sane
constexpr unsigned long PRINT_INTERVAL_MS = 1000; // debug cadence, independent of sensor servicing
   

void wakeUp()
{
    // Setting Radio mode pins to 0 to boot into normal mode - connection to software means it is configurable in the future
    pinMode(22, OUTPUT);
    digitalWrite(22, LOW); // M0
    pinMode(23, OUTPUT);
    digitalWrite(23, LOW); // M1

    Serial.begin(DEBUG_BAUD);
    GNSS_SERIAL.begin(GNSS_BAUD);
    RADIO_SERIAL.begin(RADIO_BAUD);

    uint32_t bootTime = micros();
    uint8_t bootSecondCount = 5;

    // Wait for serial debug if not initiated
    while (!Serial)
    {
    }

    // General wake-up window
    while ((micros() - bootTime) < BOOT_SEQ_DELAY)
    {
        // Give the USB serial monitor a moment to attach after upload/reset.
        Serial.print(bootSecondCount);
        delay(200);
        Serial.print(".");
        delay(200);
        Serial.print(".");
        delay(200);
        Serial.print(". ");
        bootSecondCount--;
        delay(400);
    }

    // Activate I2C lines - some libraries do it, but preemptive activation ensures that they work
    Wire.begin();
    Wire1.begin();
    Wire2.begin();
}

void sensorStartup()
{
    // Altimeter
    Serial.println(alti.begin() ? "Altimeter Detected and Started" : "Failed");

    // Assumed to be on ground during setup
    Serial.println("\nAltimeter Pressure Reading:");
    seaLevelPressurePa = alti.getPressure();
    seaLevelPressurehPa = seaLevelPressurePa / 100;
    Serial.println(seaLevelPressurehPa);

    // IMU
    Serial.println("\nIMU:");
    Serial.println(imu.begin() ? "Detected and Started" : "Failed");
    Serial.println(imu.configureForFlight() ? "Configured for flight" : "Configuration failed");

    // GNSS runs at 10Hz
    Serial.println("\nGNSS:");
    if (!gnss.begin())
    {
        Serial.println("GNSS UNAVAILABLE");
    }
}

void sensorDebug()
{
    Serial.print("Loop: ");
    Serial.println(loopCount);
    Serial.println("\n1. Altimeter Health Check:");
    printStatus(alti.checkHealth());

    Serial.print("\nAltimeter Temperature Reading: ");
    Serial.println(alti.getTemperature());

    Serial.print("\nSealevel Pressure Reading (hPa): ");
    Serial.println(seaLevelPressurehPa);
    rawPressurePa = alti.getPressure();
    pressurehPa = rawPressurePa / 100;
    Serial.print("Pressure in Pa: ");
    Serial.print(rawPressurePa);
    Serial.print("\nPressure in hPa: ");
    Serial.print(pressurehPa);

    Serial.print("\nAltimeter Altitude Reading: ");
    Serial.println(alti.getAltitude(seaLevelPressurehPa));

    Serial.println("\n---\n");

    // ---- IMU ----

    Serial.println("2. IMU Health Check:");
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

    Serial.println("3. GNSS Status: ");
    printStatus(gnss.checkHealth());
    Serial.println(); // formatting
    gnss_data = gnss.getData(gnss_data);
    gnss_data.print();

    Serial.println("\n-------------------------------------------\n");
}

void setup()
{
    wakeUp();

    Serial.println("\n\n--------------\nEntered Setup!\n--------------\n");

    sensorStartup();

    Serial.println("\n-----------\nSETUP ENDED\n-----------\n\n-------------------------------------------\n\n");
}

void loop()
{
    unsigned long now = millis();

    
}

/*
CODE SCRAPS
loop:
    // GNSS_SERIAL.write(0x55); // arbitrary test byte
    // delay(500);
    // while (GNSS_SERIAL.available())
    // {
    //     Serial.println(GNSS_SERIAL.read(), HEX);
    // }
    // while (GNSS_SERIAL.available())
    // {
    //     uint8_t b = GNSS_SERIAL.read();
    //     if (b < 0x10)
    //         Serial.print("0");
    //     Serial.print(b, HEX);
    //     Serial.print(" ");
    // }
    // Serial.println();
    // Serial3.write(0x55); // arbitrary test byte
    // delay(500);
    // while (Serial3.available())
    // {
    //     Serial.println(Serial3.read(), HEX);
    // }

*/