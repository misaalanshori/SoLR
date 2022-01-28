#include "LoopbackStream.h"
#include <SPI.h>
#include <LoRa.h>

// Pin Configuration
#define ss 15
#define rst 16
#define dio0 4

// Comment out the following line when compiling for client
// #define SoLR_HOST

#define SoLR_DEBUGMODE

// Serial Configuration
#define SoLR_BAUD 9600
#define SoLR_BUFSIZE 5120
#define SoLR_PAYLOAD_SIZE 16

// LoRa Configuration
#define SoLR_FREQ 433E6
#define SoLR_GAIN 6
#define SoLR_TXPOWER 20
#define SoLR_SPREAD 12
#define SoLR_BANDWIDTH 125E3
#define SoLR_CR 8

// Connection Configuration
#define SoLR_CONN_TO 3000UL
#define SoLR_BEACON_DELAY 1500UL
#define SoLR_PACKET_DELAY 1UL

// Flip address for Host and Client
#ifdef SoLR_HOST
byte localAddress = 0xDD;     // address of this device
byte remoteAddress = 0xBB;      // destination to send to
#else
byte localAddress = 0xBB;     // address of this device
byte remoteAddress = 0xDD;      // destination to send to
#endif

int receivedBytes = 0;
int sendFlag = 0;

unsigned long lastRecvTime = 0;
unsigned long lastBeacTime = 0;
unsigned long lastSendTime = 0;

LoopbackStream buffer(SoLR_BUFSIZE);

// Data Format
#define headerLength 3 
// 1: destination, 
// 2: localAddress, 
// 3: packet type (0x00: beacon, 0x01: regular data)
// rest: data

void sendBuffer() {
  int bufferAvailable = buffer.available();
  Serial.println("t2");
  if (bufferAvailable > SoLR_PAYLOAD_SIZE) {
    bufferAvailable = SoLR_PAYLOAD_SIZE;
  }
  byte store[bufferAvailable];
  buffer.readBytes(store, bufferAvailable);
  Serial.println("t3");
  LoRa.beginPacket();                    // start packet
  LoRa.write(remoteAddress);             // add destination address
  LoRa.write(localAddress);              // add sender address
  LoRa.write(0x01);                      // add packet type (type 0x01: regular data)
  Serial.println("t4");
  LoRa.write(store, bufferAvailable);    // add payload
  Serial.println("t5");
  LoRa.endPacket(true);                      // finish packet and send it asyncronously
  Serial.println("t6");

  lastSendTime = millis();

  #ifdef SoLR_DEBUGMODE
  Serial.print("Received This: ");
  Serial.write(store, bufferAvailable);
  Serial.println();
  Serial.println("t8");
  #endif
}

void sendBeacon() {
  LoRa.beginPacket();                    // start packet
  LoRa.write(remoteAddress);             // add destination address
  LoRa.write(localAddress);              // add sender address
  LoRa.write(0x00);                      // add packet type (type 0x00: beacon)
  LoRa.write(0xff);                      // add payload
  LoRa.endPacket(true);                      // finish packet and send it
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
  
  #ifdef SoLR_DEBUGMODE
  Serial.print(recipient, HEX);
  Serial.println(type, HEX);
  #endif

  switch (type) {
    case 0x00: // if packet is a beacon
      #ifdef SoLR_DEBUGMODE    
      Serial.println("Beacon Received!");
      #endif
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
  Serial.println("sl-2");
  int availableBytes = Serial.available();
  byte store[availableBytes];
  Serial.println("sl-3");
  Serial.readBytes(store, availableBytes);
  Serial.println("sl-4");
  buffer.write(store, availableBytes);
  Serial.println("sl-5");
}

void setReceiveFlag(int recvBytes) {
  if (recvBytes < 1) {
    return;
  }
  #ifdef SoLR_DEBUGMODE
  Serial.print("Received Something, Length: ");
  Serial.println(recvBytes);
  #endif
  receivedBytes = recvBytes;
}

void onTx() {
  LoRa.receive();
}

void setup() {
  #ifdef SoLR_DEBUGMODE
  Serial.println("Boot Up");
  #endif
  Serial.begin(SoLR_BAUD);          // initialize serial
  while (!Serial);
  #ifdef SoLR_DEBUGMODE
  Serial.println("Init LoRa");
  #endif
  LoRa.setPins(ss, rst, dio0);      // set CS, reset, IRQ pin
  if (!LoRa.begin(SoLR_FREQ)) {     // initialize ratio at freq hz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                   // if failed, do nothing
  } else {
  #ifdef SoLR_DEBUGMODE
  Serial.println("LoRa Conf");
  #endif
  LoRa.setTxPower(SoLR_TXPOWER);
  LoRa.setGain(SoLR_GAIN);
  LoRa.setSpreadingFactor(SoLR_SPREAD);
  LoRa.setSignalBandwidth(SoLR_BANDWIDTH);
  LoRa.setCodingRate4(SoLR_CR);
  // LoRa.onReceive(setReceiveFlag);
  // LoRa.onTxDone(onTx);
  // LoRa.receive();
  }
  sendBeacon();
}

void loop() {
  // #ifdef SoLR_DEBUGMODE
  // Serial.print(" | ");
  // Serial.print(Serial.available());
  // Serial.print(" | ");
  // Serial.print(receivedBytes);
  // Serial.print(" | ");
  // Serial.print(sendFlag);
  // Serial.println();
  // #endif

  setReceiveFlag(LoRa.parsePacket());

  if (Serial.available()) {
    Serial.println("sl-1");
    loadToBuffer();
    Serial.println("sl-6");
  }

  if (sendFlag == 1) {
    Serial.println("t1");
    sendBuffer();
    Serial.println("t9");

    sendFlag = 0;
  }
  
  #ifdef SoLR_HOST
  if ((millis() - lastRecvTime > SoLR_CONN_TO) && (millis() - lastBeacTime > SoLR_BEACON_DELAY)) {
    #ifdef SoLR_DEBUGMODE
    Serial.println("Sending Beacon...");
    #endif
    sendBeacon();
  }
  #endif
  
  if (receivedBytes > 1) {
    Serial.println("rv1");
    receiveData(receivedBytes);
    receivedBytes = 0;
    Serial.println("rv2");
  }

  

}
