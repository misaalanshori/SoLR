#include <SPI.h>
#include <LoRa.h>

// Pin Configuration
#define ss 15
#define rst 16
#define dio0 4

// Comment out the following line when compiling for client
#define SoLR_HOST

// Serial Configuration
#define SoLR_BAUD 1200
#define SoLR_BUFSIZE 250

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

// Flip address for Host and Client
#ifdef SoLR_HOST
byte localAddress = 0xDD;     // address of this device
byte remoteAddress = 0xBB;      // destination to send to
#else
byte localAddress = 0xBB;     // address of this device
byte remoteAddress = 0xDD;      // destination to send to
#endif

byte charBuffer[SoLR_BUFSIZE] = {0};
int bufferIndex = 0;

int receivedBytes = 0;

int sendFlag = 0;

unsigned long lastRecvTime = 0;
unsigned long lastBeacTime = 0;
unsigned long lastSendTime = 0;


// Data Format
#define headerLength 3 
// 1: destination, 
// 2: localAddress, 
// 3: packet type (0x00: beacon, 0x01: regular data)
// rest: data

void sendBuffer() {
  LoRa.beginPacket();                    // start packet
  LoRa.write(remoteAddress);             // add destination address
  LoRa.write(localAddress);              // add sender address
  LoRa.write(0x01);                      // add packet type (type 0x01: regular data)
//  Serial.write(charBuffer, bufferIndex+1);
//  Serial.println();
  LoRa.write(charBuffer, bufferIndex+1); // add payload
  memset(charBuffer, 0, SoLR_BUFSIZE);
  bufferIndex = 0;
  LoRa.endPacket();                      // finish packet and send it asyncronously
  LoRa.receive();
  lastSendTime = millis();
}

void sendBeacon() {
  LoRa.beginPacket();                    // start packet
  LoRa.write(remoteAddress);             // add destination address
  LoRa.write(localAddress);              // add sender address
  LoRa.write(0x00);                      // add packet type (type 0x00: beacon)
  LoRa.write(0xff);                      // add payload
  LoRa.endPacket();                      // finish packet and send it
  LoRa.receive();
  lastBeacTime = millis();
}

void receiveData(int packetSize) {
  if ((packetSize < headerLength + 1) || packetSize > SoLR_BUFSIZE + headerLength) return;    // if there isn't valid packet, return
  byte recipient = LoRa.read();
  byte sender = LoRa.read();
  byte type = LoRa.read();
  
  if (recipient != localAddress || sender != remoteAddress) return;
  
  byte serialBuffer[SoLR_BUFSIZE] = {0};
  int serialBufferLength = LoRa.available();
  LoRa.readBytes(serialBuffer, serialBufferLength);
  
  switch (type) {
    case 0x00: // if packet is a beacon
//      Serial.println("Beacon Received!");
      break;
      
    case 0x01: // if packet is a regular data
      Serial.write(serialBuffer, serialBufferLength);
      break;
      
    default: // if packet type is invalid
      return;
  }
  
  lastRecvTime = millis();
  sendFlag = 1;
}

void loadToBuffer() {
  int availableBytes = Serial.available();
  int toRead = 0;
  if (availableBytes >= SoLR_BUFSIZE - bufferIndex) {
    toRead = SoLR_BUFSIZE - bufferIndex;
  } else {
    toRead = availableBytes;
  }
  Serial.readBytes(&charBuffer[bufferIndex], toRead);
  bufferIndex += toRead;
}

void setReceiveFlag(int recvBytes) {
//  Serial.print("Received Something, Length: ");
//  Serial.println(recvBytes);
  receivedBytes = recvBytes;
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
  LoRa.onReceive(setReceiveFlag);
  LoRa.receive();
  }
  sendBeacon();
}

void loop() {
  if (bufferIndex < SoLR_BUFSIZE) {
    loadToBuffer();
  }
  
  #ifdef SoLR_HOST
  if ((millis() - lastRecvTime > SoLR_CONN_TO) && (millis() - lastBeacTime > SoLR_BEACON_DELAY)) {
//    Serial.println("Sending Beacon...");
    sendBeacon();
  }
  #endif
  
  if (receivedBytes > 1) {
    receiveData(receivedBytes);
    receivedBytes = 0;
  }

  if (sendFlag == 1 && millis() - lastRecvTime > SoLR_PACKET_DELAY) {
    sendBuffer();
    sendFlag = 0;
  }

}
