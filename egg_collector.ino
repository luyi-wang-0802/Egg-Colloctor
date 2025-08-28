#include <Servo.h>

/* ====== Parameters ====== */
const int STEP_DELAY_US = 100;       // Step High-Low Level Interval
const int COL_PITCH = 4.8;           // Egg Position Pitch (Unit: Steps)
const int STEPS_PER_UNIT = 1600;     // Steps per Unit (Original code directly multiplied by 1600)

/* ====== Pins ====== */
// Horizontal
const uint8_t TX_PULL = 3;
const uint8_t TX_DIR  = 5;
// Vertical
const uint8_t LG_PULL = 7;
const uint8_t LG_DIR  = 8;
// Sensors
const uint8_t TX_HOME_SW = 10;       // Horizontal home switch, triggered by LOW
const uint8_t LG_HOME_SW = 11;       // Vertical home switch, triggered by LOW
const uint8_t IR_SW      = 4;        // Infrared detection, LOW means egg present
// Servo
const uint8_t SERVO_PIN  = 8;

/* ====== Status ====== */
int txEggCnt = 0;                    // Number of eggs placed in current row
int lgRowCnt = 0;                    // Number of completed rows
Servo gripper;

/* ====== Basic Actions ====== */
inline void pulse(uint8_t pullPin, int pulses) {
  for (int i = 0; i < pulses; i++) {
    digitalWrite(pullPin, HIGH);
    delayMicroseconds(STEP_DELAY_US);
    digitalWrite(pullPin, LOW);
    delayMicroseconds(STEP_DELAY_US);
  }
}

void moveStepper(uint8_t dirPin, uint8_t pullPin, bool forward, int steps) {
  if (steps <= 0) return;
  digitalWrite(dirPin, forward ? HIGH : LOW);
  pulse(pullPin, steps);
}

/* Servo opens to drop egg, then closes */
void dropEgg() {
  for (int p = 0; p <= 20; ++p) { gripper.write(p); delay(15); }
  delay(300);
  for (int p = 20; p >= 0; --p) { gripper.write(p); delay(15); }
}

/* ====== Initialization ====== */
void setup() {
  gripper.attach(SERVO_PIN);

  pinMode(TX_PULL, OUTPUT);
  pinMode(TX_DIR,  OUTPUT);
  pinMode(LG_PULL, OUTPUT);
  pinMode(LG_DIR,  OUTPUT);

  pinMode(TX_HOME_SW, INPUT_PULLUP);
  pinMode(LG_HOME_SW, INPUT_PULLUP);
  pinMode(IR_SW,      INPUT_PULLUP);

  Serial.begin(9600);

  // Horizontal homing: move until home switch is LOW
  while (digitalRead(TX_HOME_SW) != LOW)  moveStepper(TX_DIR, TX_PULL, false, 1);
  delay(300);

  // Vertical homing: move until home switch is LOW
  while (digitalRead(LG_HOME_SW) != LOW)  moveStepper(LG_DIR, LG_PULL, true, 1);
  delay(300);

  // Move to initial position (same three-stage movement as original)
  moveStepper(TX_DIR, TX_PULL, true, 14400);
  moveStepper(LG_DIR, LG_PULL, false, 800);
  moveStepper(LG_DIR, LG_PULL, false, 32000);
  delay(500);
}

/* ====== Main Loop ====== */
void loop() {
  // Wait for infrared to detect egg in position
  while (digitalRead(IR_SW) != LOW) { delay(10); }

  // Calculate horizontal steps to target column in this row
  // Original formula: (4.8 * txEggCnt - 9.6) * 1600
  int steps = int((COL_PITCH * txEggCnt - 2 * COL_PITCH) * STEPS_PER_UNIT);

  // Move horizontally to target column
  if (steps < 0) moveStepper(TX_DIR, TX_PULL, false, -steps);
  else           moveStepper(TX_DIR, TX_PULL, true,  steps);
  delay(200);

  // Drop egg
  dropEgg();
  delay(200);

  // Move horizontally back to starting column
  if (steps < 0) moveStepper(TX_DIR, TX_PULL, true,  -steps);
  else           moveStepper(TX_DIR, TX_PULL, false, steps);
  delay(200);

  // Update counters
  txEggCnt += 1;

  // Move to next row when current row is full (5 eggs per row)
  if (txEggCnt >= 5) {
    txEggCnt = 0;
    lgRowCnt += 1;

    // If tray not full, move vertically to next row (displacement = 4.8 units)
    if (lgRowCnt < 6) {
      moveStepper(LG_DIR, LG_PULL, true, int(4.8 * STEPS_PER_UNIT));
    } else {
      // Tray is full: execute original "emergency stop" strategy
      while (true) { delay(1000); }  // Explicitly stop loop
    }
  }
}