#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>   
 
#define POWER 9                 //power pin to reduce consumption while sleeping
 
// TODO: store this in EEPROM
#define DEVICE_ID 1
 
#define CHANNEL 1 //MAX 127
RF24 radio(2, 15);              // Set up nRF24L01 radio on SPI bus plus pins 7 & 8
int tempVal = 0; // Variable to store the temp value coming from the sensor.
int voltVal = 0; // Variable to store the calculated current voltage value.
 
// Topology
// FIXME: can this be stored in EEPROM?
const uint64_t pipes[2] = { 0xFFFFFFFFFFLL, 0xCCCCCCCCCCLL };
 
// Adjust this value to your board's specific internal BG voltage
// This needs to be accurate to set per board to be able to report current battery voltage
// TODO: store this in EEPROM
const long InternalReferenceVoltage = 1078;
 
// Uncomment this line if you are using the updated dallas_temp_library that
// supports the busFail() method to diagnose bus problems
// #define BUSFAIL
 
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 6
#define TEMPERATURE_PRECISION 9
#define TEMP_SENSOR_ID 0
   
 
struct dataStruct {
  unsigned long _salt;
  float temp;
  int volt;
} data;
 
void setup() {
  pinMode(POWER, OUTPUT);
 
  Serial.begin(115200); 
  restartRadio();
  radio.printDetails();
  data._salt = 0;
}
 float tempf =28.1;
 int tempi=1;
void loop() {
  tempf = tempf +0.1;
   tempi = tempi +1;
  
  // get all sensor values and increment the salt so we have at least one unique value in the payload
  data.volt = tempi;
  Serial.printf("Voltage: %d\r\n", data.volt);
  data.temp = tempf;
  char tempTemp[10];
  dtostrf(data.temp, 4, 2, tempTemp);
  Serial.printf("Temperature: %s\r\n", tempTemp);
  data._salt++;
  Serial.printf("Salt: %d\r\n", data._salt);
  Serial.printf("------------------\r\n");
 
  // send sensor data to gateway
  restartRadio();                    // turn on and configure radio
  Serial.printf("Now sending %d byte payload... ", sizeof(data));
  if (!radio.write(&data , sizeof(data) )) { // Send via radio
    Serial.printf("failed.\n\r");
  } else {
    Serial.printf("sent.\n\r");
  }
  stopRadio();                       // stop the radio
 
  // Enter power down state for 8 s with ADC and BOD module disabled
  Serial.println("Sleeping...");
  Serial.flush(); 
  Serial.printf("waking up...\r\n");
}
 
/*
* Get temperature
*/
 
 
void stopRadio() {
 
  radio.powerDown();
 
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
 
  digitalWrite(POWER, LOW);  // remove power from nrf24l01+ module
}
 
void restartRadio() {
  digitalWrite(POWER, HIGH);         // provide power to nrf24l01+ module
 
  radio.begin(); // Start up the radio
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipes[1]);
  radio.openWritingPipe(pipes[0]);
  //  radio.openReadingPipe(1,pipes[0]);
  //  radio.openWritingPipe(pipes[1]);
  radio.stopListening();
}
 
/********
* This tests the Dallas One-Wire bus
* Only works with the updated dallas_temp_library that supports the
* budFail() method to diagnose bus problems
********/
void testBus() {
  #ifdef BUSFAIL
    Serial.print(" Test:");
    if (sensors.reset()) {
      Serial.print("good");
    } else {
      if (sensors.busFail()) {
        Serial.print("fail");
      } else {
        Serial.print("empty");
      }
    }
  #endif
}
