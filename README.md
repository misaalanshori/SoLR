**Rewrite Branch**
# Serial over LoRa
A full serial line connection between 2 devices using LoRa.

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
1. Clone the repo
2. Upload the code to one of the modules, this module will be the host
3. Comment out the `#define SoLR_HOST` Line, and Upload that to the second module. This module will be the client
4. Connect both modules to a serial terminal and if they connected properly you can start sending text/data between the 2 modules

## Libraries used
- [sandeepmistry/arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa)
- [misaalanshori/ArduinoBufferedStreams](https://github.com/misaalanshori/ArduinoBufferedStreams) (Fork of [paulo-raca/ArduinoBufferedStreams](https://github.com/paulo-raca/ArduinoBufferedStreams))

## To do
Make the code better than the previous code...

## Misc
### About the ArduinoBufferedStreams fork
I added a write(buf, size) method to it, though I'm not 100% sure if it works perfectly as I'm not really a pro C++ programmer yet :P. So i haven't tried to make a PR to merge it with the original repo. Though I tested it as best I could and it seem to work, so now I'm using it for this project.