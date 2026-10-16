# Wiring and Pin Mapping (ESP32 additions)

This supplements the earlier wiring for Arduino. When using an ESP32 DevKitC, use the following suggestions.

ESP32 pin suggestions (change as needed for your board):

Motor driver (L298N) to ESP32
- ENA (motor A PWM) -> ESP32 GPIO 18 (PWM channel 0)
- IN1 -> ESP32 GPIO 14
- IN2 -> ESP32 GPIO 27

- ENB (motor B PWM) -> ESP32 GPIO 19 (PWM channel 1)
- IN3 -> ESP32 GPIO 26
- IN4 -> ESP32 GPIO 25

Ultrasonic sensor (HC-SR04)
- TRIG -> ESP32 GPIO 13
- ECHO -> ESP32 GPIO 12

Joystick (optional)
- VRx -> ESP32 GPIO 32 (ADC1)
- VRy -> ESP32 GPIO 33 (ADC1)
- SW -> ESP32 GPIO 15 (optional)

Bluetooth
- The sketch uses the ESP32 Bluetooth SPP via the BluetoothSerial library. No external BT module is required when using ESP32.

Power
- Motors: motor battery -> L298N VMOT
- ESP32: 5V (via USB) or regulated 5V -> 5V pin
- Common ground required between motor battery, L298N, and ESP32

Notes
- The L298N is power-hungry and inefficient; for higher-current motors prefer an H-bridge like the TB6612FNG, VNH2SP30, or a motor driver with MOSFETs.
- The ESP32's 3.3V IO is native; ensure motor driver logic is compatible.
