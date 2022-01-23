#include <SPI.h>
#include <LoRa.h>
#include <string.h>
#include <stdint.h>

#define ss 15
#define rst 16
#define dio0 4

#define baud 300
#define freq 433E6
#define bufsize 120

byte localAddress = 0xDD;     // address of this device
byte destination = 0xBB;      // destination to send to

byte charBuffer[bufsize] = {0};
int bufferIndex = 0;
unsigned long lastRecvTime = 0;

void loadToBuffer() {
  while (Serial.available()) {
    charBuffer[bufferLength] = Serial.read();
    bufferIndex++;
  }
}

void sendBuffer() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(0x01)                      // type 0x01: regular data
  LoRa.write(charBuffer, bufferIndex+1);// add payload
  memset(charBuffer, 0, bufsize);
  bufferIndex = 0;
  LoRa.endPacket();                     // finish packet and send it
}

void sendBeacon() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(0x00)                      // type 0x00: beacon
  LoRa.write(0xff);                     // add payload
  LoRa.endPacket();                     // finish packet and send it
}

void setup() {
  Serial.begin(baud);          // initialize serial
  while (!Serial);

  LoRa.setPins(ss, rst, dio0); // set CS, reset, IRQ pin
  if (!LoRa.begin(freq)) {     // initialize ratio at freq hz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);              // if failed, do nothing
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
