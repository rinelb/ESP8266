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
boolean connectedtobroker = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  
  pinMode(4,OUTPUT);
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
   Serial.println("++++++++BEGIN+++++++++++");
   Serial.println("");
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println("");
  Serial.print ("message = ");

  for (int i = 0; i < length; i++) {
                Serial.print((char)payload[i]);
               // jsonstring[i] = (char)payload[i];
                //Serial.print( jsonstring[i]);
               // MQTTmessage = MQTTmessage + (char)payload[i];
            }
    Serial.println("    ");
     Serial.print("  Atentication  "); 
     Serial.print(Atentication); 

  
  MQTTmessage = "";
  MQTTtopic = topic;
  
   //char json[length-1];


 if (MQTTtopic.equals("controller-thing1/activation/result") && !Atentication )
    {
       //Atentication = true;
        Serial.println(""); 
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
                Serial.print((char)payload[i]);
                jsonstring[i] = (char)payload[i];
                //Serial.print( jsonstring[i]);
                MQTTmessage = MQTTmessage + (char)payload[i];
            }
               
               Serial.println("*******"); 
               
               /*
               Serial.print("   jsonstring =   #");
              Serial.print(jsonstring[length]);
              Serial.print(jsonstring[length+1]);
              Serial.println("#");
              Serial.print (" MQTTmessage - ");
              Serial.println(MQTTmessage); 
          
              Serial.print("before If Statment  json2 =   [");
              Serial.print(jsonstring);
              Serial.println("]");*/

   
             Serial.println("before json object create"); 
              JsonObject& root = jsonBuffer.parseObject(jsonstring);
              Serial.println("after json object create"); 
      
              // Test if parsing succeeds.
              if (!root.success()) {
                Serial.println("parseObject()  FAILED!!");
             
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
  
         if (MQTTtopic.equals("controller-thing1/type.microcontroller/led-light-status/room1/actuation") && Atentication)
         {
                   Serial.println("  Inside LED actuation  ");
                  if((char)payload[0] == 'o' && (char)payload[1] == 'n') //on
                  {
                    Serial.print("ON DETECTED");
                    digitalWrite(4,HIGH);
                    ledvalue = "on";
                    client.publish("controller-thing1/type.microcontroller/led-light-status/room1/data","on");
                  }
                  else if((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') //off
                   {
                    digitalWrite(4,LOW);
                    //Serial.print("OFF DETECTED");
                    ledvalue = "off";
                    client.publish("controller-thing1/type.microcontroller/led-light-status/room1/data","off");
                   }
        


       
              Serial.println();

         }

         Serial.println("++++++++END+++++++++++");
}




void reconnect() {
        // Loop until we're reconnected
        while (!client.connected()) {
          Serial.print("Attempting MQTT connection...");
          // Attempt to connect
          if (client.connect("controller-thing1","controller-thing1/status",2,0,"offline")) {
              mqttConnected = true;  
              Serial.println("Connected");
              Serial.println("sent Attentication sent Succefully waiting  "); 
               //delay(10000);
              connectedtobroker = true;
            
            
              
            //client.subscribe("controller-thing1/type.microcontroller/temperature-event/room1/actuation");
            //client.subscribe("controller-thing1/status"); 
            //client.publish("controller-thing1/activation", "{\"uid\" : \"controller-thing1\"}");
            //client.publish("controller-thing1/activation", "{\"uid\" : \"controller-thing1\", \"activate\":true, \"resources\":[{\"thingUid\":\"controller-thing1\", \"thingType\":\"type.microcontroller\", \"resourceTitle\":\"room1\", \"resourceTypes\":[\"led-light-status\"], \"contentType\":\"0\", \"contentFormat\":\"on off\", \"observable\":true},{ \"thingUid\":\"controller-thing1\", \"thingType\":\"type.microcontroller\", \"resourceTitle\":\"temperatureevent\", \"resourceTypes\":[\"temperature-event\"], \"contentType\":\"0\", \"contentFormat\":\"number\", \"observable\":true}]}");
            //client.publish("controller-thing1/activation", "{\"uid\" : \"controller-thing1\", \"activate\":true}");


            client.subscribe("controller-thing1/type.microcontroller/led-light-status/room1/actuation");
            //client.subscribe("controller-thing1/status");
            client.subscribe("controller-thing1/activation/result");
            client.subscribe("controller-thing1/+/+/+/observation");



            
              
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
          Serial.print("ADC - Val ");
          Serial.print(sensorValue);
          Serial.println("");
          if (sensorValueold != sensorValue){
            
            //Serial.println("connected");
            //Serial.println(sensorValue);
            //Serial.print(" Publishing to controller-thing1/type.microcontroller/temperature-event/room1 - ");
            //Serial.print(String(sensorValue).c_str());
            
            //client.publish("ESP8266/rinel", String(sensorValue).c_str());
        
            client.publish("controller-thing1/type.microcontroller/temperature-event/temperatureevent/data",  String(sensorValue).c_str());
            //Serial.println(" ");
            sensorValueold = sensorValue;


             if (ledvalue.equals("on")){
              client.publish("controller-thing1/type.microcontroller/led-light-status/room1/data","on");
            }else {
              client.publish("controller-thing1/type.microcontroller/led-light-status/room1/data","off");
            }


             Serial.print("LED - Val ");
             Serial.print(ledvalue);
              Serial.println("");

            
          }



          if (ledvalueold.equals(ledvalue)){
          }
          else{

            //Serial.print(String(sensorValue).c_str());
            client.publish("controller-thing1/type.microcontroller/temperature-event/temperatureevent/data",  String(sensorValue).c_str());


            
            //Serial.print("ledvalue - ");
            //Serial.println(ledvalue);
            //Serial.print(" Publishing to controller-thing1/type.microcontroller/temperature-event/room1 - ");
            //Serial.print(String(sensorValue).c_str());
            
            //client.publish("ESP8266/rinel", String(sensorValue).c_str());
            if (ledvalue.equals("on")){
              client.publish("controller-thing1/type.microcontroller/led-light-status/room1/data","on");
            }else {
              client.publish("controller-thing1/type.microcontroller/led-light-status/room1/data","off");
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
            
            //client.publish("controller-thing1/activation", "{\"uid\" : \"controller-thing1\"}");
            client.publish("controller-thing1/activation",  "{\"uid\" : \"controller-thing1\",  \"activate\":true,  \"resources\":[  {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"LED\",  \"resourceTypes\":[\"window-state-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"door\",  \"resourceTypes\":[\"door-state-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"amperes\",  \"resourceTypes\":[\"electric-current-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"speed\",  \"resourceTypes\":[\"speed-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"acceleration\",  \"resourceTypes\":[\"accelerometer-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"temperature\",  \"resourceTypes\":[\"temperature-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"humidity\",  \"resourceTypes\":[\"humidity-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"air-quality\",  \"resourceTypes\":[\"air-quality-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"light\",  \"resourceTypes\":[\"light-level-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},    {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"sound\",  \"resourceTypes\":[\"sound-level-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},    {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"space\",  \"resourceTypes\":[\"space-availability-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},    {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"motion\",  \"resourceTypes\":[\"motion-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},  { \"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"indoor-location\",  \"resourceTypes\":[\"indoor-location-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"string\",  \"observable\":true}]}");          

  
}


void senddata()
{
             
    int randomnumber;
    randomnumber = random(0,1032);
    char* onoff ="on";
    if (randomnumber<512)
    {
      onoff = "on";
    }else
    {
       onoff = "off";
    }
    client.publish("controller-thing1/type.microcontroller/window-state-event/window/data", onoff );


   randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/door-state-event/door/data", onoff );
 
 
   randomnumber = random(0,1032);

  client.publish("controller-thing1/type.microcontroller/electric-current-event/amperes/data",  String(randomnumber).c_str());

 randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/speed-event/speed/data",  String(randomnumber).c_str());
 


   randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/accelerometer-event/acceleration/data",  String(randomnumber).c_str());
 


 randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/temperature-event/temperature/data",  String(randomnumber).c_str());
 

randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/humidity-event/humidity/data", String(randomnumber).c_str() );
 
 
 randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/air-quality-event/air-quality/data",  String(randomnumber).c_str());



  randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/light-level-event/light/data",  String(randomnumber).c_str());


 randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/sound-level-event/sound/data",  String(randomnumber).c_str());


  randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/space-availability-event/space/data", onoff );



   randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/motion-event/motion/data", onoff );



    randomnumber = random(0,1032);
  client.publish("controller-thing1/type.microcontroller/indoor-location-event/indoor-location/data", onoff );
         
 
 


 










 
}



void loop() {
 
          if (!client.connected()) {
            
            //Atentication = false;
            reconnect();
             
          }
 
           

          
          client.loop();
          delay(15000);
          if (Atentication){
            //sendtemp();
            senddata();
          }

          if (mqttConnected && (!Atentication)){
       
          //sendAtentication();
            
               //Serial.println("RESENDING   sendAtentication() ");
             
            //client.publish("controller-thing1/activation", "{\"uid\" : \"controller-thing1\"}");
             client.publish("controller-thing1/activation",  "{\"uid\" : \"controller-thing1\",  \"activate\":true,  \"resources\":[  {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"window\",  \"resourceTypes\":[\"window-state-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"door\",  \"resourceTypes\":[\"door-state-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"amperes\",  \"resourceTypes\":[\"electric-current-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"speed\",  \"resourceTypes\":[\"speed-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"acceleration\",  \"resourceTypes\":[\"accelerometer-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"temperature\",  \"resourceTypes\":[\"temperature-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"humidity\",  \"resourceTypes\":[\"humidity-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"air-quality\",  \"resourceTypes\":[\"air-quality-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},   {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"light\",  \"resourceTypes\":[\"light-level-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},    {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"sound\",  \"resourceTypes\":[\"sound-level-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"number\",  \"observable\":true},    {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"space\",  \"resourceTypes\":[\"space-availability-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},    {\"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"motion\",  \"resourceTypes\":[\"motion-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"on off\",  \"observable\":true},  { \"thingUid\":\"controller-thing1\",  \"thingType\":\"type.microcontroller\",  \"resourceTitle\":\"indoor-location\",  \"resourceTypes\":[\"indoor-location-event\"],  \"contentType\":\"0\",  \"contentFormat\":\"string\",  \"observable\":true}]}");          

  
          //client.publish("controller-thing1/activation", "{\"uid\" : \"controller-thing1\", \"activate\":true}");

            
          }

  
}

