/*
  RC_Car_ESP32.ino
  ESP32 sketch for a 2-wheel differential robot using an H-bridge (L298N or similar).
  Features:
  - Bluetooth SPP (BluetoothSerial) control from phone
  - Optional joystick analog control
  - HC-SR04 ultrasonic obstacle detection and simple avoidance (stop + back up + turn)
  - Smooth acceleration (ramping)

  Commands (over Bluetooth serial):
  - 'F' or 'f' : forward (optional speed 0-255, e.g., F200)
  - 'B' or 'b' : backward (optional speed)
  - 'L' or 'l' : spin left
  - 'R' or 'r' : spin right
  - 'S' or 's' : stop
*/

#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Motor pins (change if needed)
const int ENA = 18; // PWM channel 0
const int IN1 = 14;
const int IN2 = 27;

const int ENB = 19; // PWM channel 1
const int IN3 = 26;
const int IN4 = 25;

// PWM settings
const int PWM_FREQ = 20000; // 20 kHz
const int PWM_RES = 8; // 8-bit -> 0-255
const int PWM_CH_A = 0;
const int PWM_CH_B = 1;

// Joystick pins (optional)
const int JOY_X = 32; // ADC1
const int JOY_Y = 33;
const int JOY_BTN = 15;

// Ultrasonic sensor pins
const int US_TRIG = 13;
const int US_ECHO = 12;

// Control parameters
const int JOY_DEADZONE = 60;
const int MAX_PWM = 255;
const int OBSTACLE_CM = 20; // threshold distance in cm

// acceleration/ramp
int currentLeft = 0;
int currentRight = 0;
const int RAMP_STEP = 10; // pwm units per control step

unsigned long lastControlMillis = 0;
const int CONTROL_INTERVAL = 20;

void setup() {
  // pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ultrasonic
  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);

  // joystick button
  pinMode(JOY_BTN, INPUT_PULLUP);

  // setup PWM channels
  ledcSetup(PWM_CH_A, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENA, PWM_CH_A);
  ledcSetup(PWM_CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENB, PWM_CH_B);

  Serial.begin(115200);
  SerialBT.begin("RC_Car_ESP32"); // Bluetooth device name

  stopMotors();
  Serial.println("RC Car ESP32 ready");
}

void loop() {
  unsigned long now = millis();
  if (now - lastControlMillis < CONTROL_INTERVAL) return;
  lastControlMillis = now;

  // obstacle check
  int dist = readUltrasonicCM();
  if (dist > 0 && dist < OBSTACLE_CM) {
    // obstacle: perform simple avoidance
    Serial.printf("Obstacle %d cm - avoiding\n", dist);
    stopMotors();
    delay(100);
    // back up a little
    setMotorA(-150);
    setMotorB(-150);
    delay(300);
    // spin right a bit
    setMotorA(150);
    setMotorB(-150);
    delay(300);
    stopMotors();
    currentLeft = currentRight = 0;
    return;
  }

  // Bluetooth control takes precedence
  if (SerialBT.available()) {
    String cmd = SerialBT.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 0) {
      Serial.print("BT: "); Serial.println(cmd);
      handleBTCommand(cmd);
    }
    return;
  }

  // Joystick fallback
  int rawX = analogRead(JOY_X);
  int rawY = analogRead(JOY_Y);
  int x = rawX - 2048; // ESP32 ADC is 0-4095
  int y = rawY - 2048;

  // deadzone
  if (abs(x) < JOY_DEADZONE*4) x = 0;
  if (abs(y) < JOY_DEADZONE*4) y = 0;

  int forward = map(y, -2048, 2047, -MAX_PWM, MAX_PWM);
  int turn = map(x, -2048, 2047, -MAX_PWM, MAX_PWM);

  int leftTarget = constrain(forward + turn, -MAX_PWM, MAX_PWM);
  int rightTarget = constrain(forward - turn, -MAX_PWM, MAX_PWM);

  // ramping towards target speeds for smoothness
  currentLeft = rampTo(currentLeft, leftTarget);
  currentRight = rampTo(currentRight, rightTarget);

  setMotorA(currentLeft);
  setMotorB(currentRight);
}

int rampTo(int current, int target) {
  if (current == target) return current;
  if (abs(target - current) <= RAMP_STEP) return target;
  if (target > current) return current + RAMP_STEP;
  return current - RAMP_STEP;
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
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : 200;
      currentLeft = currentRight = sp;
      setMotorA(sp);
      setMotorB(sp);
      break;
    }
    case 'B': case 'b': {
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : 200;
      currentLeft = currentRight = -sp;
      setMotorA(-sp);
      setMotorB(-sp);
      break;
    }
    case 'L': case 'l': {
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : 180;
      setMotorA(-sp);
      setMotorB(sp);
      currentLeft = -sp; currentRight = sp;
      break;
    }
    case 'R': case 'r': {
      int sp = (value >= 0) ? constrain(value, 0, MAX_PWM) : 180;
      setMotorA(sp);
      setMotorB(-sp);
      currentLeft = sp; currentRight = -sp;
      break;
    }
    case 'S': case 's': {
      stopMotors();
      currentLeft = currentRight = 0;
      break;
    }
    default: {
      stopMotors();
      currentLeft = currentRight = 0;
      break;
    }
  }
}

// Motor A (left)
void setMotorA(int speed) {
  if (speed == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    ledcWrite(PWM_CH_A, 0);
    return;
  }
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    ledcWrite(PWM_CH_A, speed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    ledcWrite(PWM_CH_A, -speed);
  }
}

// Motor B (right)
void setMotorB(int speed) {
  if (speed == 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    ledcWrite(PWM_CH_B, 0);
    return;
  }
  if (speed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    ledcWrite(PWM_CH_B, speed);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(PWM_CH_B, -speed);
  }
}

void stopMotors() {
  setMotorA(0);
  setMotorB(0);
}

int readUltrasonicCM() {
  // send 10us pulse
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);

  unsigned long duration = pulseIn(US_ECHO, HIGH, 30000); // timeout 30ms
  if (duration == 0) return -1;
  int cm = duration / 58; // approximate
  return cm;
}
