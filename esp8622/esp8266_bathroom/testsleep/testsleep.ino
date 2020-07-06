/**
 * An example showing how to put ESP8266 into Deep-sleep mode
 */
 
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
 delay(500);
  
  Serial.println("I'm awake.");
delay(500);
  Serial.println("Going into deep sleep for 20 seconds");
  ESP.deepSleep(20e6); // 20e6 is 20 microseconds
  Serial.println("does this run");
}

void loop() {
  Serial.println("does this run");
  delay(500);
}
