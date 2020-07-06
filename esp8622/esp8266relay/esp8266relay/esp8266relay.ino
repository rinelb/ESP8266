#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "HUAWEI-B315-0365";
const char* password = "232TE5TG2G2";

//const char* mqtt_server = "test.mosquitto.org";
//const char* mqtt_server = "iot.eclipse.org";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
//const char* mqtt_server = "broker.hivemq.com";
//const char* mqtt_server = "ssfbase2.meraka.csir.co.za";
const char* mqtt_server = "gen3broker.meraka.csir.co.za";


// Memory pool for JSON object tree.
//
// Inside the brackets, 200 is the size of the pool in bytes,
// If the JSON object is more complex, you need to increase that value.
// See https://bblanchon.github.io/ArduinoJson/assistant/
StaticJsonBuffer<250> jsonBuffer;


int sensorPin = A0;    // select the input pin for the potentiometer

int ledPin = 2; // GPIO2 of ESP8266
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensorValueold = -1; 
char* sendreading;
String MQTTmessage = "";
String MQTTtopic = "";
boolean mqttConnected = false;
String ledvalue= "off";
String ledvalueold = "on";
 
boolean Atentication = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  
  pinMode(4,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  MQTTmessage = "";
  MQTTtopic = topic;
  
   //char json[length-1];

 if (MQTTtopic.equals("controller-thing3/activation/result"))
    {
       Atentication = true;
        //Serial.println(""); 
        //Serial.print("topic reciever in if statment");
                /* char json[] =
                  "{\"foood\" : true,\"time\" : 1351824120,\"data\" : [48.756080,2.302038]}";
            
                  Serial.print("display  json =   [");
                Serial.print(json);
                Serial.println("]");
            
              // Root of the object tree.
              //
              // It's a reference to the JsonObject, the actual bytes are inside the
              // JsonBuffer with all the other nodes of the object tree.
              // Memory is freed when jsonBuffer goes out of scope.
              JsonObject& root = jsonBuffer.parseObject(json);
            
              // Test if parsing succeeds.
              if (!root.success()) {
                Serial.println("parseObject() failed");
                 
              }else{
                 Serial.println("parseObject() Wroked!!!!");
              }
             
               */

              char jsonstring[length+1];
             for (int i = 0; i < length; i++) {
              //Serial.print((char)payload[i]);
              jsonstring[i] = (char)payload[i];
              //Serial.print( jsonstring[i]);
              MQTTmessage = MQTTmessage + (char)payload[i];
            }
               
              //Serial.println("*******"); 
              // Serial.print("   jsonstring =   #");
              //Serial.print(jsonstring[length]);
              //Serial.print(jsonstring[length+1]);
              //Serial.println("#");
              //Serial.print (" MQTTmessage - ");
              //Serial.println(MQTTmessage); 
          
              //Serial.print("before If Statment  json2 =   [");
              //Serial.print(jsonstring);
              //Serial.println("]");

   
        
              JsonObject& root = jsonBuffer.parseObject(jsonstring);
      
              // Test if parsing succeeds.
              if (!root.success()) {
                //Serial.println("parseObject()2 failed");
             
              }else{Serial.println("parseObject() working");}
              boolean isittrue = root["success"];
              if (root["success"] ){
                Serial.println("  Atentication 2 Done!!!"); 
                //client.publish("ESP8266/rinel", "Connected!");
                Atentication = true;
              }

       
            }else
            {
               Serial.println("  not equals  "); 
            }
  
         if (MQTTtopic.equals("controller-thing3/type.microcontroller/led-light-status/room1/actuation") && Atentication)
          
                  if((char)payload[0] == 'o' && (char)payload[1] == 'n') //on
                  {
                    Serial.print("ON DETECTED");
                    digitalWrite(4,HIGH);
                    digitalWrite(6,HIGH);
                    digitalWrite(7,HIGH);
                    ledvalue = "on";
                    client.publish("controller-thing3/type.microcontroller/led-light-status/room1/data","on");
                  }
                  else if((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') //off
                   {
                    digitalWrite(4,LOW);
                    digitalWrite(6,LOW);
                    digitalWrite(7,LOW);
                    //Serial.print("OFF DETECTED");
                    ledvalue = "off";
                    client.publish("controller-thing3/type.microcontroller/led-light-status/room1/data","off");
                   }
        


       
              Serial.println();
}




void reconnect() {
        // Loop until we're reconnected
        while (!client.connected()) {
          Serial.print("Attempting MQTT connection...");
          // Attempt to connect
          if (client.connect("controller-thing3","controller-thing3/status",2,0,"offline")) {
              mqttConnected = true;  
              Serial.println("Connected");

               Serial.println("inside sendAtentication() ");

            client.subscribe("controller-thing3/type.microcontroller/led-light-status/room1/actuation");
            //client.subscribe("controller-thing3/status");
            client.subscribe("controller-thing3/activation/result");
            client.subscribe("controller-thing3/+/+/+/observation");
            
              
            //client.subscribe("controller-thing3/type.microcontroller/temperature-event/room1/actuation");
            //client.subscribe("controller-thing3/status"); 
            //client.publish("controller-thing3/activation", "{\"uid\" : \"controller-thing3\"}");
            //client.publish("controller-thing3/activation", "{\"uid\" : \"controller-thing3\", \"activate\":true, \"resources\":[{\"thingUid\":\"controller-thing3\", \"thingType\":\"type.microcontroller\", \"resourceTitle\":\"room1\", \"resourceTypes\":[\"led-light-status\"], \"contentType\":\"0\", \"contentFormat\":\"on off\", \"observable\":true}},{ \"thingUid\":\"controller-thing3\", \"thingType\":\"type.microcontroller\", \"resourceTitle\":\"temperatureevent\", \"resourceTypes\":[\"temperature-event\"], \"contentType\":\"0\", \"contentFormat\":\"number\", \"observable\":true}]}");
         //client.publish("controller-thing3/activation", "{\"uid\" : \"controller-thing3\", \"activate\":true}");
 
              
          } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
          }
        }
}


void sendtemp() {
          sensorValue = analogRead(sensorPin);
          Serial.print("ADC - Val");
          //Serial.print(sensorValue);
          //if (sensorValueold != sensorValue){
            
            //Serial.println("connected");
            //Serial.println(sensorValue);
            //Serial.print(" Publishing to controller-thing3/type.microcontroller/temperature-event/room1 - ");
            //Serial.print(String(sensorValue).c_str());
            
            //client.publish("ESP8266/rinel", String(sensorValue).c_str());
        
            client.publish("controller-thing3/type.microcontroller/temperature-event/temperatureevent/data",  String(sensorValue).c_str());
            //Serial.println(" ");
            sensorValueold = sensorValue;


             if (ledvalue.equals("on")){
              client.publish("controller-thing3/type.microcontroller/led-light-status/room1/data","on");
            }else {
              client.publish("controller-thing3/type.microcontroller/led-light-status/room1/data","off");
            }


            
          //}



          if (ledvalueold.equals(ledvalue)){
          }
          else{

            //Serial.print(String(sensorValue).c_str());
            client.publish("controller-thing3/type.microcontroller/temperature-event/temperatureevent/data",  String(sensorValue).c_str());


            
            //Serial.print("ledvalue - ");
            //Serial.println(ledvalue);
            //Serial.print(" Publishing to controller-thing3/type.microcontroller/temperature-event/room1 - ");
            //Serial.print(String(sensorValue).c_str());
            
            //client.publish("ESP8266/rinel", String(sensorValue).c_str());
            if (ledvalue.equals("on")){
              client.publish("controller-thing3/type.microcontroller/led-light-status/room1/data","on");
            }else {
              client.publish("controller-thing3/type.microcontroller/led-light-status/room1/data","off");
            }
            //Serial.println(" ");
            ledvalueold = ledvalue;
          }


          
          //Serial.println(" ");
  
}


void sendAtentication()
{
             
            // Once connected, publish an announcement...
            //client.publish("ESP8266/rinel", "Connected!");
            // ... and resubscribe
            //Serial.println("inside sendAtentication() ");
            
            //client.publish("controller-thing3/activation", "{\"uid\" : \"controller-thing3\"}");
            client.publish("controller-thing3/activation", "{\"uid\" : \"controller-thing3\", \"activate\":true, \"resources\":[{\"thingUid\":\"controller-thing3\", \"thingType\":\"type.microcontroller\", \"resourceTitle\":\"room1\", \"resourceTypes\":[\"led-light-status\"], \"contentType\":\"0\", \"contentFormat\":\"on off\", \"observable\":true},{ \"thingUid\":\"controller-thing3\", \"thingType\":\"type.microcontroller\", \"resourceTitle\":\"room1\", \"resourceTypes\":[\"temperature-event\"], \"contentType\":\"0\", \"contentFormat\":\"number\", \"observable\":true}]}");
         

  
}

void loop() {
 
          if (!client.connected()) {
            Atentication = false;
            reconnect();
          }
          client.loop();
          delay(6000);
          if (Atentication){
            sendtemp();
          }

          if (mqttConnected && (!Atentication)){
       
          //sendAtentication();
            
               //Serial.println("RESENDING   sendAtentication() ");
             
            //client.publish("controller-thing3/activation", "{\"uid\" : \"controller-thing3\"}");
            client.publish("controller-thing3/activation", "{\"uid\":\"controller-thing3\",\"activate\":true,\"resources\":[{\"thingUid\":\"controller-thing3\",\"thingType\":\"type.microcontroller\",\"resourceTitle\":\"room1\",\"resourceTypes\":[\"led-light-status\"],\"contentType\":\"0\",\"contentFormat\":\"onoff\",\"observable\":true},{\"thingUid\":\"controller-thing3\",\"thingType\":\"type.microcontroller\",\"resourceTitle\":\"temperatureevent\",\"resourceTypes\":[\"temperature-event\"],\"contentType\":\"0\",\"contentFormat\":\"number\",\"observable\":true}]}");
          
          //client.publish("controller-thing3/activation", "{\"uid\" : \"controller-thing3\", \"activate\":true}");

            
          }

  
}

