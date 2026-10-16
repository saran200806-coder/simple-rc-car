# Simple RC Car

A beginner-friendly remote-control car project. This update improves control logic, adds obstacle detection (ultrasonic HC-SR04), supports ESP32 (Wi‑Fi/Bluetooth), and includes a minimal Android Bluetooth app to control the car from a smartphone.

Highlights
- Improved motor ramping and mixing for smoother driving
- Obstacle detection with HC-SR04 (stops and backs up when obstacle detected)
- ESP32 sketch using Bluetooth SPP (BluetoothSerial) for phone control and Wi‑Fi placeholders
- Minimal Android app (Kotlin) that discovers paired Bluetooth devices and sends simple commands

Repository layout (added files)
- src/RC_Car_ESP32.ino - ESP32-compatible sketch with ultrasonic sensor and BT control
- mobile/android/ - Minimal Android app (Kotlin) to connect/send commands
- hardware/wiring.md - updated with ESP32 wiring and ultrasonic wiring
- components.md - updated BOM

Quick start (ESP32 + Bluetooth)
1. Flash `src/RC_Car_ESP32.ino` to an ESP32 (e.g., ESP32 DevKitC).
2. Wire motors, L298N, HC-SR04, and power per `hardware/wiring.md`.
3. Pair your phone with the ESP32 Bluetooth (it appears as the module name specified in the sketch).
4. Open the Android app, connect to the ESP32, and use the buttons or joystick UI to drive.

If you want, I can now:
- polish the Android app UI and add a joystick control
- add a web-based controller using ESP32 Wi‑Fi (WebSocket or HTTP endpoints)
- add motor encoder support for closed-loop speed control

