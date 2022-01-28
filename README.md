# Serial over LoRa
A full serial line connection between 2 devices using LoRa. Currently the code is set up for 1200bps with a 250bytes buffer, in theory we should be able to get up to 2000bps safely without extra buffering code that i would need to figure out first. Though Linux console only works at 300bps currently (not yet sure why)

The hardware is 2 pairs of a Wemos ESP8266 module connected to a Ra-02 SX1278 module with the following pin connection

|NodeMCU Pins|SX1278 Pins|
|---|---|
|GND|GND|
|3.3V|VCC|
|D8|NSS|
|D7|MOSI|
|D6|MISO|
|D5|SCK|
|D0|RST|

## How to use
0. Connect your LoRa module according to the above configuration or reconfigure with your own setup
1. Clone the repo (or just the one ino file)
2. Upload the code to one of the modules, this module will be the host
3. Comment out the `#define SoLR_HOST` Line, and Upload that to the second module. This module will be the client
4. Connect both modules to a serial terminal and if they connected properly you can start sending text/data between the 2 modules

## Libraries used
- [sandeepmistry/arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa)

## Things I wanna (try to) do
- Address Filtering
- Encryption
- Error Correction
- Resending (most) Lost Packets
- Re-Configuration Packets (to change configuration on the fly)
- Secondary Serial connection for control and configuration? (using SoftwareSerial)
- Make it compatible with the Arduino Serial library with Stream methods

## Things I need to fix
- The damn thing transmits 100% of the time and basically just jams the 433mhz frequency.
- Some reliability problem like random crashes etc.
- Linux console doesnt seem to work at baud rates other than 300bps. I tested 600 and 1200 bps and it seems to just be sending/receiving garbled data. But even at 300bps, its actually not super terrible. Though regular data/text transmission seem to work perfectly at these higher baud rates, even when both sides have data being sent at the same time. 

## Misc
Command I used to run a linux shell/console on a ttyUSB:
```
sudo getty -L **baudrate** **tty** **terminal type** -8 -a **username**

For example:
sudo getty -L 600 ttyUSB0 vt102 -a adminispwd
```