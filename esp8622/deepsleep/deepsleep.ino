/**
 * An example showing how to put ESP8266 into Deep-sleep mode
 */
 
#include <ESP8266WiFi.h> 
  char* ssid = "HUAWEI-B315-0365";
    //char* ssid="smart_campus";
  char* password = "232TE5TG2G2";
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
//setup_wifi();
  // Wait for serial to initialize.
 delay(5000);
  
  Serial.println("I'm awake.");
delay(500);
  Serial.println("Going into deep sleep for 20 seconds");
  ESP.deepSleep(20e6,WAKE_RF_DEFAULT); // 20e6 is 20 microseconds
  Serial.println("does this run");
}

void loop() {
  Serial.println("does this run");
  delay(500);
}


void setup_wifi(){

  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);
  int count_connections = 0;
  while (WiFi.status() != WL_CONNECTED && count_connections < 10) {
    delay(500);
    //Serial.print(".");
    count_connections++;
  }
  if (WiFi.status() == WL_CONNECTED){
   
  

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
  }else { Serial.println("wifi failed");};
  
}

