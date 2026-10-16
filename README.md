# Simple RC Car

A beginner-friendly remote-control car project using an Arduino, two DC motors, and an L298N motor driver. Supports joystick control (analog) and optional Bluetooth control (HC-05/HC-06).

Features
- Forward / Reverse / Left / Right via joystick or Bluetooth
- PWM speed control
- Simple wiring and BOM for beginners

Repository layout
- README.md - Overview and quick start
- hardware/wiring.md - Wiring and pin mapping
- components.md - Bill of materials (BOM)
- src/RC_Car.ino - Arduino sketch supporting joystick and Bluetooth

Quick start
1. Assemble motors and motor driver following wiring.md.
2. Upload src/RC_Car.ino to your Arduino Uno/Nano.
3. Use a 2-axis analog joystick connected to A0/A1 for steering and throttle, or pair a phone via a Bluetooth serial terminal app to the HC-05/HC-06 module.

