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

byte localAddress = 0xBB;     // address of this device
byte destination = 0xDD;      // destination to send to



void sendBuffer() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void setup() {
  Serial.begin(baud);          // initialize serial
  while (!Serial);

  LoRa.setPins(ss, rst, dio0); // set CS, reset, IRQ pin
  if (!LoRa.begin(freq)) {     // initialize ratio at freq hz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);              // if failed, do nothing
  }

  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.println("LoRa init succeeded.");
}

void loop() {
  // put your main code here, to run repeatedly:

}
