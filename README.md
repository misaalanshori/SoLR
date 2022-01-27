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
4. Connect both modules to a serial terminal and if they successfully connect then you can start sending text/data between the 2 modules

## Libraries used
- [sandeepmistry/arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa)

## Things I wanna (try to) do
- Address Filtering **(❌ Not happening, 1 connection per channel already very slow)**
- Encryption **(❔ Planned)**
- Error Correction **(❔ Planned, Also the sx1278 already has FEC, so maybe no?)**
- Resending (most) Lost Packets **(❔ Planned)**
- Re-Configuration Packets (to change configuration on the fly) **(❔ Planned)**
- Web Configuration/Status Page **(❔ Planned)**
- Make it compatible with the Arduino Serial library with Stream methods **(❔ Planned, But this is kinda the end game plan)**
- Create some Profiles for the configuration to prioritize bitrate or range **(❔ Planned)**

## Things I need to fix
- The damn thing transmits 100% of the time and basically just jams the 433mhz frequency. **The Plan: if there are no inputs or received payloads after a couple seconds and the code would go into idle mode where each packets are more spread out. I feel this still isn't a very good solution, but its the best I can think of**

- Some reliability problem like random crashes etc. **This is probably gonna stay here for ever**

- Linux console doesnt seem to work at baud rates other than 300bps. I tested 600 and 1200 bps and it seems to just be sending/receiving garbled data. But even at 300bps, its actually not super terrible. Though regular data/text transmission seem to work perfectly at these higher baud rates, even when both sides have data being sent at the same time. **I have no idea why this problem exists**

- The host times out when sending long packets. **(❗❗❗ Important Fix!) The Plan: limit each packet payload to something small like under 32bytes. this also means that we need to add a more advanced buffering system. Planning to use paulo-raca/ArduinoBufferedStreams. This also has a possibility of fixing the linux console problem**

## Misc
Command I used to run a linux shell/console on a ttyUSB:
```
sudo getty -L **baudrate** **tty** **terminal type** -8 -a **username**

For example:
sudo getty -L 300 ttyUSB0 vt102 -a adminispwd
```