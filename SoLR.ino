#include <SPI.h>
#include <LoRa.h>
#include "LoopbackStream.h"

#define SoLR_CONFIG
#ifdef SoLR_CONFIG
// Pin Configuration
#define ss 15
#define rst 16 //16
#define dio0 4

// Comment out the following line when compiling for client
// #define SoLR_HOST
#define SoLR_DEBUG

// Serial Configuration
#define SoLR_BAUD 9600
#define SoLR_BUFSIZE 5120

// LoRa Configuration
#define SoLR_FREQ 433E6
#define SoLR_GAIN 6
#define SoLR_TXPOWER 20
#define SoLR_SPREAD 12
#define SoLR_BANDWIDTH 125E3
#define SoLR_CR 8

// Connection Configuration
#define SoLR_CONN_TO 2250UL
#define SoLR_BEACON_DELAY 2500UL
#define SoLR_PACKET_DELAY 1UL
#define SoLR_HEADER_SIZE 3
#define SoLR_ID 0xFB
#define SoLR_PACKET_FOOTER 0x4A
#define SoLR_MAX_PAYLOAD_SIZE 16

#endif

#define SoLR_PACKET_TYPES
#ifdef SoLR_PACKET_TYPES
#define SoLR_BEACON_PACKET 0x00
#define SoLR_DATA_PACKET 0x01
#endif

LoopbackStream buffer(SoLR_BUFSIZE);

byte sendingBuf[SoLR_MAX_PAYLOAD_SIZE];
byte recvBuf[SoLR_MAX_PAYLOAD_SIZE];

unsigned long long lastRecvTime = 0;
unsigned long long lastSendTime = 0;

bool sendAvailable = true;
short sendingNow = 0;
int sendSize = 0;
void sendBuffer(byte type, byte *buf, byte size) {
  if (size > SoLR_MAX_PAYLOAD_SIZE) return;

  LoRa.beginPacket();

  // Write Header
  LoRa.write(SoLR_ID);
  LoRa.write(type);
  LoRa.write(size);   

  // Write Data
  if (size > 0) {
    LoRa.write(buf, size);
  }
  

  // Write Footer
  LoRa.write(SoLR_PACKET_FOOTER);
  LoRa.endPacket();
  lastSendTime = millis();
  sendAvailable = false;
  LoRa.receive();
}

void onReceive(int packetSize) {
  
  if (LoRa.available() < SoLR_HEADER_SIZE) return;

  byte cID = LoRa.read();
  if (cID != SoLR_ID) return;
  byte type = LoRa.read();
  byte size = LoRa.read();
  if (size > SoLR_MAX_PAYLOAD_SIZE) return;

  if (size > 0 ) {
    LoRa.readBytes(recvBuf, size);
  }

  byte footer = LoRa.read();

  if (footer != SoLR_PACKET_FOOTER) return;
  switch (type)
  {
  case SoLR_BEACON_PACKET:
    break;
  
  case SoLR_DATA_PACKET:
    Serial.write(recvBuf, size);
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
  LoRa.receive();
  }
  
}

void loop() {

  loadToBuffer(Serial.available());

  
  if ((millis() - lastSendTime > SoLR_PACKET_DELAY ) && sendAvailable) {

    sendSize = buffer.available() > SoLR_MAX_PAYLOAD_SIZE ? SoLR_MAX_PAYLOAD_SIZE : buffer.available();
    buffer.readBytes(sendingBuf, sendSize);
    sendBuffer(SoLR_DATA_PACKET, sendingBuf, sendSize);
  }
  if (sendingNow == 0) {
    onReceive(LoRa.parsePacket());
  }
  

  #ifdef SoLR_HOST
  if ((millis() - lastRecvTime > SoLR_CONN_TO) && (millis() - lastSendTime > SoLR_BEACON_DELAY)) {

    sendBuffer(SoLR_BEACON_PACKET, sendingBuf, 0);
  }
  #endif
}
