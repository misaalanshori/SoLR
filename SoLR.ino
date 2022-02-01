#include <SPI.h>
#include <LoRa.h>
#include "LoopbackStream.h"

#define SoLR_CONFIG
#ifdef SoLR_CONFIG
// Pin Configuration
#define ss 15
#define rst 16
#define dio0 4

// Comment out the following line when compiling for client
// #define SoLR_HOST
#define SoLR_DEBUG

// Serial Configuration
#define SoLR_BAUD 1200
#define SoLR_BUFSIZE 5120

// LoRa Configuration
#define SoLR_FREQ 433E6
#define SoLR_GAIN 6
#define SoLR_TXPOWER 20
#define SoLR_SPREAD 12
#define SoLR_BANDWIDTH 125E3
#define SoLR_CR 8

// Connection Configuration
#define SoLR_CONN_TO 2000UL
#define SoLR_BEACON_DELAY 1750UL
#define SoLR_PACKET_DELAY 1UL
#define SoLR_HEADER_SIZE 3
#define SoLR_ID 0xFB
#define SoLR_PACKET_FOOTER 0x4A
#define SoLR_MAX_PAYLOAD_SIZE 32

#endif

#define SoLR_PACKET_TYPES
#ifdef SoLR_PACKET_TYPES
#define SoLR_BEACON_PACKET 0x00
#define SoLR_DATA_PACKET 0x01
#endif

LoopbackStream buffer(SoLR_BUFSIZE);

byte sendingBuf[SoLR_MAX_PAYLOAD_SIZE];

unsigned long long lastRecvTime = 0;
unsigned long long lastSendTime = 0;

bool transmitReady = true;
bool sendAvailable = false;
int availableBytes = 0;

void sendBuffer(byte type, byte *buf, byte size) {
  if (size > SoLR_MAX_PAYLOAD_SIZE) return;
  transmitReady = false;

  LoRa.beginPacket();

  // Write Header
  LoRa.write(SoLR_ID);
  LoRa.write(type);
  LoRa.write(size);

  // Write Data
  LoRa.write(buf, size);

  // Write Footer
  LoRa.write(SoLR_PACKET_FOOTER);

  LoRa.endPacket(true);

  lastSendTime = millis();

}

void onReceive() {
  if (LoRa.available() < SoLR_HEADER_SIZE) return;
  
  byte cID = LoRa.read();
  if (cID != SoLR_ID) return;
  byte type = LoRa.read();
  byte size = LoRa.read();
  if (size > SoLR_MAX_PAYLOAD_SIZE) return;

  byte dataBuf[size];
  LoRa.readBytes(dataBuf, size);
  byte footer = LoRa.read();
  if (footer != SoLR_PACKET_FOOTER) return;

  switch (type)
  {
  case SoLR_BEACON_PACKET:
    break;
  
  case SoLR_DATA_PACKET:
    Serial.write(dataBuf, size);
    break;

  default:
    return;
  }

  lastRecvTime = millis();
  sendAvailable = true;
  
  
}

void loadToBuffer(int availableBytes) {
  if (!availableBytes) return;
  byte bf[availableBytes];
  Serial.readBytes(bf, availableBytes);
  buffer.write(bf, availableBytes);
}

void setTransmitReady() {
  transmitReady = true;
  LoRa.receive();
}

void setPacketAvailable(int size) {
  availableBytes = size;
}

void setup() {
  Serial.begin(SoLR_BAUD);          // initialize serial
  while (!Serial);
  
  LoRa.setPins(ss, rst, dio0);      // set CS, reset, IRQ pin
  if (!LoRa.begin(SoLR_FREQ)) {     // initialize ratio at freq hz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                   // if failed, do nothing
  } else {
  LoRa.setTxPower(SoLR_TXPOWER);
  LoRa.setGain(SoLR_GAIN);
  LoRa.setSpreadingFactor(SoLR_SPREAD);
  LoRa.setSignalBandwidth(SoLR_BANDWIDTH);
  LoRa.setCodingRate4(SoLR_CR);

  LoRa.onTxDone(setTransmitReady);
  LoRa.onReceive(setPacketAvailable);
  }
  
}

void loop() {
  loadToBuffer(Serial.available());

  if ((millis() - lastSendTime > SoLR_PACKET_DELAY ) && transmitReady && sendAvailable) {
    int sendSize = buffer.available() > SoLR_MAX_PAYLOAD_SIZE ? SoLR_MAX_PAYLOAD_SIZE : buffer.available();
    buffer.readBytes(sendingBuf, sendSize);
    sendBuffer(SoLR_DATA_PACKET, sendingBuf, sendSize);
  }

  #ifdef SoLR_HOST
  if ((millis() - lastRecvTime > SoLR_CONN_TO) && (millis() - lastSendTime > SoLR_BEACON_DELAY) && transmitReady) {
    sendBuffer(SoLR_BEACON_PACKET, sendingBuf, 0);
  }
  #endif

  if (availableBytes) {
    availableBytes = 0;
    onReceive();
  }

}
