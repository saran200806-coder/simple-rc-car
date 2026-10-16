# Wiring and Pin Mapping

This file describes the wiring for a basic 2-wheel differential RC car using an L298N motor driver and an Arduino Uno/Nano.

Components and suggested pins
- Arduino Uno/Nano
- L298N motor driver
- 2x DC motors (left and right)
- Joystick (2-axis analog)
- Optional: HC-05 / HC-06 Bluetooth module
- Battery pack (7.4V Li-ion or 6V-12V battery depending on motors)

Recommended pin connections (in the Arduino sketch `src/RC_Car.ino`):

Motor driver (L298N) to Arduino
- ENA (motor A PWM) -> Arduino D5 (PWM)
- IN1 -> Arduino D8
- IN2 -> Arduino D9

- ENB (motor B PWM) -> Arduino D6 (PWM)
- IN3 -> Arduino D10
- IN4 -> Arduino D11

Joystick
- VRx -> A0 (JOY_X)
- VRy -> A1 (JOY_Y)
- SW (button) optional -> D7

Bluetooth (HC-05/HC-06)
- HC-05 TX -> Arduino D2 (SoftwareSerial RX)
- HC-05 RX -> Arduino D3 (SoftwareSerial TX) -- be careful with voltage levels; HC-05 RX expects 3.3V signals. Use a voltage divider on Arduino TX.
- HC-05 VCC -> 5V
- HC-05 GND -> GND

Power
- Motor power (12V or battery pack) -> L298N +12V (VMOT)
- L298N GND -> Arduino GND (common ground)
- L298N 5V logic: many L298N boards have an onboard 5V regulator; you can use the onboard 5V to power Arduino if appropriate, but
  it's safer to power Arduino from USB or a separate 5V regulator. Ensure all grounds are common.

Safety notes
- Check motor stall currents and ensure the motor driver can handle them.
- Provide decoupling capacitors on motor power or use an RC filter to reduce noise.
- If you use a Li-ion battery, follow safe charging and protection practices.

