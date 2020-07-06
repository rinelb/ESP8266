//#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WiFi.h"
 char* ssid = "HUAWEI-B315-0365";
 char* password = "232TE5TG2G2";

//const char* mqtt_server = "test.mosquitto.org";
//const char* mqtt_server = "iot.eclipse.org";
//const char* mqtt_server = "broker.mqtt-dashboard.com";

//working code
//const char* mqtt_server = "broker.hivemq.com";
 char* mqtt_server = "iotvirt9.meraka.csir.co.za";
 int value;
 char msg[50];
 
void callback(char* topic, byte* payload, unsigned int length);

WiFiClient espClient;
PubSubClient client(mqtt_server,1883,callback,espClient);

void setup() {
  
  pinMode(4,OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  reconnect();
}

void setup_wifi(){

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  if((char)payload[0] == 'o' && (char)payload[1] == 'n') //on
    digitalWrite(4,HIGH);
  else if((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') //off
    digitalWrite(4,LOW);
    
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //if (client.connect("ESP8266Clientrinel")) {
    if (client.connect("kapua-sys","kapua-sys","kapua-password")) {  
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("ESP8266/rinel", "Connected!");
      // ... and resubscribe
      Serial.println("subscribe sent to \"kapua-sys/b43c-weather-station/type.weatherstation/wind-direction-event/winddirection\"" );
      client.subscribe("kapua-sys/b43c-weather-station/type.weatherstation/wind-direction-event/winddirection");
      client.subscribe("kapua-sys/rinel/test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
 
  if (!client.connected()) {
    reconnect();
  }else { delay(5000); 
          
          ++value;
          snprintf(msg, 75, "Data rinel #%ld", value);
          client.publish("kapua-sys/rinel/test", msg);
          
        }
  client.loop();
}
