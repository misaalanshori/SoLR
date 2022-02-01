#include <SPI.h>
#include <LoRa.h>

#define CONFIG
#ifdef CONFIG
// Pin Configuration
#define ss 15
#define rst 16
#define dio0 4

// Comment out the following line when compiling for client
#define SoLR_HOST

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

byte channelID  = 0xFB;

#endif

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
  }
  
}

void loop() {

}
