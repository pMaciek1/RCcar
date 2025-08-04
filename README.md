# Vehicle
The whole system works with ESP-NOW. Mac addresses are hard-coded. Upon powering each device, they are looking for the specific MAC address.
<img width="807" height="623" alt="obraz" src="https://github.com/user-attachments/assets/93353d76-0590-4f56-9a33-6626f6a8ba7b" />


# Transmitter
### Modules used:
- ESP32 DEVKIT V1
- Joystick
- LCD Screen
- Powerbank

### Description
Powered by the powerbabank with Micro USB on ESP32. Every module is powered via 3v3 pin on microcontroller.
Sgnal from joystick is processed to be 0-255 in each axis and sent to the receiver.
Transmitter has LCD screen to show the distance from the nearest obstacle.

# Receiver
### Modules used:
- ESP32 DEVKIT V1
- L298n module
- HC-SR04 module
- 8 AA bateries
- 4 DC motors

### Description
Powered by 8 AA bateries connected in series.
HC-SR04 gathers data about distance from the nearest obstacle.
Module L298n controls DC motors.
