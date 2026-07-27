/*
pin defs, constants, baud rates, PID gains
*/

#pragma once

static constexpr HardwareSerial &GNSS_SERIAL = Serial3; // Serial3: TX3=14, RX3=15

// pins
// Servos - fins (through raceway to lower stage)
constexpr int SERVO1_PIN = 2;
constexpr int SERVO2_PIN = 3;
constexpr int SERVO3_PIN = 4;
constexpr int SERVO4_PIN = 5;
constexpr int PYRO_PIN = 6; // gate drive to MOSFET, not direct pyro power — TBD later

// GPS
//constexpr int GNSS_SERIAL = Serial3; // Serial3: TX3=14, RX3=15

// Radio
constexpr int RADIO_SERIAL = 5; // Serial5: RX5=20, TX5=21
constexpr int RADIO_M0_PIN = 22;
constexpr int RADIO_M1_PIN = 23;

// I2C buses — avionics bay, one sensor per bus (no contention)
// Wire  (SDA=18, SCL=19) -> LSM6DSO IMU
// Wire1 (SDA=17, SCL=16) -> MLX90393 magnetometer
// Wire2 (SDA=25, SCL=24) -> BMP280 altimeter   // TODO: verify SDA/SCL order against PJRC pinout card before soldering

// constexpr int STATUS_LED_PIN = 13;  // Basic Debug

// tunable gains
// MENTOR's gains, WILL CHANGE IN FUTURE
constexpr double MAX_FIN_DEFLECTION_DEG = 14.0;
constexpr double ROLL_KP = 0.5, ROLL_KI = 0.0, ROLL_KD = 0.02;
constexpr double PITCH_KP = 2.0, PITCH_KI = 0.0, PITCH_KD = 0.2;
constexpr double YAW_KP = 2.0, YAW_KI = 0.0, YAW_KD = 0.2;

// baud rates
constexpr long GNSS_BAUD = 38400;
constexpr long RADIO_BAUD = 9600; // check your E32 model's default, adjust if different
constexpr long DEBUG_BAUD = 115200;