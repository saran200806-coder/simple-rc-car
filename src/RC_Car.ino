/*
  RC_Car.ino
  Simple Arduino sketch for a 2-wheel differential RC car using L298N.
  Supports analog joystick (A0/A1) and optional Bluetooth HC-05/HC-06 serial control.

  Behavior:
  - If Bluetooth serial data is available, commands from Bluetooth take precedence.
  - Otherwise, the joystick controls forward/back and steering.

  Bluetooth commands (ASCII over serial):
  - 'F' or 'f' : forward (followed by optional speed 0-255, e.g., F150)
  - 'B' or 'b' : backward (optional speed)
  - 'L' or 'l' : turn left (optional intensity)
  - 'R' or 'r' : turn right
  - 'S' or 's' : stop
  - Example: "F200" sets both motors forward at PWM 200
*/

#include <Arduino.h>
#include <SoftwareSerial.h>

// Motor pins
const int ENA = 5;  // PWM for left motor (motor A)
const int IN1 = 8;
const int IN2 = 9;

const int ENB = 6;  // PWM for right motor (motor B)
const int IN3 = 10;
const int IN4 = 11;

// Joystick pins
const int JOY_X = A0; // steering
const int JOY_Y = A1; // throttle
const int JOY_BTN = 7; // optional button

// Bluetooth software serial
const int BT_RX_PIN = 2; // Arduino RX <- HC-05 TX
const int BT_TX_PIN = 3; // Arduino TX -> HC-05 RX (use voltage divider)
SoftwareSerial bt(BT_RX_PIN, BT_TX_PIN);

// Control parameters
const int JOY_DEADZONE = 60; // analog center deadzone
const int MAX_PWM = 255;

void setup() {
  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Joystick
  pinMode(JOY_BTN, INPUT_PULLUP);

  // Serial for debugging
  Serial.begin(9600);
  bt.begin(9600);

  stopMotors();
}

void loop() {
  // If Bluetooth has data, parse and act on it
  if (bt.available()) {
    String cmd = bt.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 0) {
      Serial.print("BT cmd: "); Serial.println(cmd);
      handleBTCommand(cmd);
    }
    return; // prioritize bluetooth commands when present
  }

  // Joystick control (fallback)
  int rawX = analogRead(JOY_X); // 0..1023
  int rawY = analogRead(JOY_Y);

  int x = rawX - 512; // -512..+511
  int y = rawY - 512; // -512..+511

  // deadzone
  if (abs(x) < JOY_DEADZONE) x = 0;
  if (abs(y) < JOY_DEADZONE) y = 0;

  // convert to -255..255
  int forward = map(y, -512, 511, -MAX_PWM, MAX_PWM); // forward positive when pushing joystick up depends on your joystick wiring
  int turn = map(x, -512, 511, -MAX_PWM, MAX_PWM);

  int leftSpeed = constrain(forward + turn, -MAX_PWM, MAX_PWM);
  int rightSpeed = constrain(forward - turn, -MAX_PWM, MAX_PWM);

  setMotorA(leftSpeed);
  setMotorB(rightSpeed);

  delay(20);
}

void handleBTCommand(String cmd) {
  if (cmd.length() == 0) return;
  char c = cmd.charAt(0);
  int value = -1;
  if (cmd.length() > 1) {
    String num = cmd.substring(1);
    num.trim();
    if (num.length() > 0) value = num.toInt();
  }

  switch (c) {
    case 'F': case 'f': {
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : MAX_PWM;
      setMotorA(sp);
      setMotorB(sp);
      break;
    }
    case 'B': case 'b': {
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : MAX_PWM;
      setMotorA(-sp);
      setMotorB(-sp);
      break;
    }
    case 'L': case 'l': {
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : 150;
      // spin left: left backward, right forward
      setMotorA(-sp);
      setMotorB(sp);
      break;
    }
    case 'R': case 'r': {
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : 150;
      // spin right
      setMotorA(sp);
      setMotorB(-sp);
      break;
    }
    case 'S': case 's': {
      stopMotors();
      break;
    }
    default: {
      // unknown: stop
      stopMotors();
      break;
    }
  }
}

// Motor A (left)
void setMotorA(int speed) {
  if (speed == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    return;
  }
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -speed);
  }
}

// Motor B (right)
void setMotorB(int speed) {
  if (speed == 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
    return;
  }
  if (speed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -speed);
  }
}

void stopMotors() {
  setMotorA(0);
  setMotorB(0);
}
