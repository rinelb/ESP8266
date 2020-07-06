#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEVICE_ID 2
#define CHANNEL 1 //MAX 127

// SPI pin configuration figured out from here:
// http://d.av.id.au/blog/esp8266-hardware-spi-hspi-general-info-and-pinout/
RF24 radio(2, 15);    // Set up nRF24L01 radio on SPI bus plus pins 2 for CE and 15 for CSN

// Topology
const uint64_t pipes[2] = { 0xFFFFFFFFFFLL, 0xCCCCCCCCCCLL };

struct dataStruct{
  unsigned long _salt;
  float temp;
  int volt;
}sensor_data;

void setup() {
  yield();
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("starting......");
  restartRadio();                    // turn on and configure radio  
  Serial.println("restarting radio");
  radio.startListening();
  Serial.println("Listening for sensor values..."); 
}

void loop() {
  if (radio.available()) {
    while (radio.available()) {
      yield();
      radio.read(&sensor_data, sizeof(sensor_data));
      Serial.println("Got message:");
      Serial.print("_salt: ");
      Serial.println(sensor_data._salt);
      Serial.print("volt: ");
      Serial.println(sensor_data.volt);
      Serial.print("temp: ");
      Serial.println(sensor_data.temp);
      Serial.println("-----------");
    }
  }
}

void restartRadio(){
  yield();
  radio.begin(); // Start up the radio
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);  
  //radio.openReadingPipe(1,pipes[1]); 
  //radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[0]);
  radio.openWritingPipe(pipes[1]);  
  radio.stopListening();
}