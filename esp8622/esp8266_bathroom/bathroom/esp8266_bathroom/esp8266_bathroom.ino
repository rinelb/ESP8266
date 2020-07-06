/*
   * EEPROM Write
   *
   * Stores values read from analog input 0 into the EEPROM.
   * These values will stay in the EEPROM when the board is
   * turned off and may be retrieved later by another sketch.
   */

   extern "C" {
 #include "user_interface.h"  // Required for wifi_station_connect() to work
}
#define FPM_SLEEP_MAX_TIME 0xFFFFFFF
   #include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "kurapayload.pb.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h> 
 #include <WiFiUdp.h>
#include <TimeLib.h>
#include <PubSubClient.h>
#include <NTPClient.h>
  #include <Arduino.h>
  #include <EEPROM.h>
void WiFiOn();
void WiFiOff();

/***********   Kura decoder variables*/
 uint8_t gotPayload [1280];
 int  gotPayloadLength ;
 uint8_t metricBuffer[1280];
int metricBufferCount = 0;
int metricCount = 0; 
int metricArryI[30];
String metricArryS[30];
bool decoded = false;
int callbackcount = 0;

kuradatatypes_KuraPayload_KuraMetric decodem = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)3, false, 0, false,0, false,0, true, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
/************************* WiFi Access Point *********************************/

  char* ssid = "HUAWEI-B315-0365";
    //char* ssid="smart_campus";
  char* password = "232TE5TG2G2";
/************************* MQTT  Setup *********************************/

  void callback(char* topic, byte* payload, unsigned int length);
const char* mqtt_server = "iotvirt9.meraka.csir.co.za";
WiFiClient espClient;
//PubSubClient client(espClient);
PubSubClient client(mqtt_server,1883,callback,espClient); 

bool decodeCompleted=true;
int buffer2Lenght;
bool sentbirthmessage = false;

int changeval = 25;

/************ Global State (you don't need to change this!) ******************/


/************************************ Time ****************************************/
int64_t unixTime;  
time_t ntp_time=0;
bool gotTime = false;
WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

char *recievedMqttMessage;

bool mqttRecived = false;





uint32_t x=0;
// Initialize kuradatatypes_KuraPayload_KuraMetric
kuradatatypes_KuraPayload_KuraMetric valores = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, true, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}}; 

kuradatatypes_KuraPayload_KuraPosition devicePosition = {26.21548, -28.3512, true, 1203, false, 0, false, 0, false, 0, false, 1, false, 0, false, 0};

// ########################  struct declaration for the birthmessage  ####################################
kuradatatypes_KuraPayload_KuraMetric display_name = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric model_id = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric part_number = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric serial_number = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric firmware_version = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric bios_version = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric os = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric os_version= {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric model_name = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)5, false, 0, false,0, false,0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};

//field tag name
const char *display_name_field = "display_name";
const char *model_id_field = "model_id";
const char *part_number_field = "part_number";
const char *serial_number_field = "serial_number"; 
const char *firmware_version_field = "firmware_version";
const char *bios_version_field = "bios_version";
const char *os_field = "os";
const char *os_version_field = "os_version";
const char *model_name_field = "model_name";

//field tag value
const char *display_name_v = "CSIR Bathroom";
const char *model_id_v = "1234567";
const char *part_number_v = "01";
const char *serial_number_v = "10000001"; 
const char *firmware_version_v = "1.0";
const char *bios_version_v = "1.0";
const char *os_v = "Tensilica Xtensa LX6";
const char *os_version_v = "1.0";
//const char *model_name_v = "kapua-sys/b4311-139-tissue/type.dispensor/relative-rotational-angle/Metric-Dispensor";

// ########################  struct declaration for the DATA  ####################################
kuradatatypes_KuraPayload_KuraMetric light_metric = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)3, false, 0, false,0, false,0, true, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric sound_metric = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)3, false, 0, false,0, false,0, true, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric air_metric = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)3, false, 0, false,0, false,0, true, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric temp_metric = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)3, false, 0, false,0, false,0, true, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};
kuradatatypes_KuraPayload_KuraMetric humd_metric = {{{NULL}, NULL}, (kuradatatypes_KuraPayload_KuraMetric_ValueType)3,  false, 0, false,0, false,0, true, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}};






 String device_id ;

 String thing_id = "b4311-139-tissue";

//const String device_id = "b4311-139-tissue";


//String topic_birthmessage ="$EDC/kapua-sys/" +  device_id + "/MQTT/BIRTH";
//String topic_data = "kapua-sys/" +  device_id + "/type.dispensor/relative-rotational-angle/Metric-Dispensor";
//String topic_actuation = "$EDC/kapua-sys/b4311-139-tissue/type.building.government.room.shared.kitchen/actuator-on-off/b4311-142-actuator-on-off";







  
  
  // the current address in the EEPROM (i.e. which byte
  // we're going to write to next)
  int amin = 32;
  int amax = 64;
  int acurrent = 20;

  int LED =0;
  
int actualVal;
 int minVal;
  int maxVal ;
  int setCurrent; 
  int current;
  int inhigh = 5;
  int inlow = 4;
double percentage;


  
uint64_t timeValue = 1523609427241;  



char * uintToStr( const uint64_t num, char *str )
{
  uint8_t i = 0;
  uint64_t n = num;
  
  do
    i++;
  while ( n /= 10 );
  
  str[i] = '\0';
  n = num;
 
  do
    str[--i] = ( n % 10 ) + '0';
  while ( n /= 10 );

  return str;
}

  
  bool wifiConnect;

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
    wifiConnect = true;
  

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
  }else { wifiConnect = false;Serial.println("wifi failed");};
  
}



void reconnect() {
        // Loop until we're reconnected
        while (!client.connected()) {
          //Serial.print("Attempting MQTT connection...");
          // Attempt to connect
          if (client.connect(thing_id.c_str(),"kapua-sys", "kapua-password")) {
             
            //Serial.println("Connected");
               
            
             //Serial.println("Sending subcribtion ");
             
            //client.subscribe("kapua-sys/Generate-EDC-Client5/DATA/value",2);
            
            //client.subscribe(topic_actuation.c_str());
            //client.subscribe("kapua-sys/#");

            //Serial.println("Sending subcribtion completed ");

            
              
          } else {
            //Serial.print("failed, rc=");
            //Serial.print(client.state());
            //Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
          }
        }
}



//Reconnect always
void _keepAlive() {
  
  while (!client.connected()) {
    //Serial.print(F("Contacting Server on: "));
    //Serial.print(mqtt_server);
    //Serial.println();
  if (client.connect(thing_id.c_str(),"kapua-sys", "kapua-password")) {
             
            //Serial.println("Connected");
               
            
             //Serial.println("Sending subcribtion ");
                         
            //client.subscribe(topic_actuation.c_str());
      
     
     // Serial.println(topic_data.c_str());Serial.println("subscribe succufull ");
   
         
    }
    else {
      //Serial.print("Failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" Retrying...");
      delay(6000);
     // encode_data();
    }
  }
}



   bool highSet;
    bool lowSet;
   int difference;

char model_name_v[700];

String topic_birthmessage;
String topic_data;

   
  void setup()
  {

  
    
    Serial.begin(115200);
     pinMode(inhigh,INPUT);
        //pinMode(LED,OUTPUT);
     pinMode(inlow,INPUT); 
  Serial.setTimeout(2000);
  delay(100);
   Serial.println("started ");
 Serial.println("started ");
 // setup_wifi();
    EEPROM.begin(320);

     actualVal = analogRead(0);
      current = EEPROM.read(acurrent);
      minVal = EEPROM.read(amin)*4;
      maxVal = EEPROM.read(amax)*4;

     int sum = 0; 
      int samples = 30;
      for (int i=0; i<samples ; i++)
      {
        sum= sum + analogRead(0);
        delay(1);
      }
      actualVal = round(sum/samples);

       int numrator = actualVal - minVal;
       int denoinator = maxVal - minVal;
         percentage = (double)numrator/(double)denoinator*100;
      difference = abs(round(percentage) - current);
      //Serial.printf("difference value: %u\n", difference);
       
       
       if (digitalRead(inhigh)==HIGH ){
        highSet = true;
       }else {highSet=false;}
       if (digitalRead(inlow)==HIGH ){
        lowSet = true;
       }else {lowSet=false;}
       
       //Serial.print("low  ");
        //Serial.println(lowSet);
      //Serial.print("high  ");
        //Serial.println(highSet);
      
      if (!lowSet && !highSet)
       {
         if  (difference > 3 ){
  WiFiOn();

            setup_wifi();

         device_id = WiFi.macAddress();
"kapua-sys/b4311-139-tissue/type.dispensor/relative-rotational-angle/Metric-Dispensor";
  String model_v = "kapua-sys/" +  device_id +"/"+thing_id+ "/type.dispensor/relative-rotational-angle/Metric-Dispensor";
  //take string called model_v and converts it to char array
  model_v.toCharArray(model_name_v, model_v.length()+1);
  topic_birthmessage ="$EDC/kapua-sys/"+thing_id+ "/MQTT/BIRTH";
  topic_data = "kapua-sys/" +  device_id +"/"+thing_id+"/type.dispensor/relative-rotational-angle/Metric-Dispensor";

  //topic_actuation = "$EDC/kapua-sys/" +  device_id + "/"+thing_id+"/type.building.government.room.shared.kitchen/actuator-on-off/" +  thing_id + "-actuator-on-off";


  Serial.print("mac_Address - ");
  Serial.println(device_id);

  Serial.print("model_name_v - ");
  Serial.print(model_name_v);

  Serial.print("topic_birthmessage - ");
  Serial.println(topic_birthmessage);

  Serial.print("topic_data - ");
  Serial.println(topic_data);
 

            
           if (wifiConnect == false)
           {
             //Serial.println("retry wifi ");
             setup_wifi();
               
              client.setServer(mqtt_server, 1883);
              client.setCallback(callback);}
          else{
                client.setServer(mqtt_server, 1883);
              client.setCallback(callback);}
          
         }
       }
     //Serial.println("setup");
  }
  
  void loop()
  {

    //Serial.println("I'm awake.");
 //blink led
   if (!lowSet && !highSet)
       {
        //Serial.println("check difference");
         if  (difference > 3 ){
                /*digitalWrite(LED,LOW);
                delay(1000);
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);*/


                
               
                
                // need to divide by 4 because analog inputs range from
                // 0 to 1023 and each byte of the EEPROM can only hold a
                // value from 0 to 255. 
                 
                 
                  
                    //minVal  =  1000;
               
                setCurrent = (int)percentage;
                //Serial.printf("setCurrent  value: %u\n", setCurrent);
                 
                 
                // write the value to the appropriate byte of the EEPROM.
                // these values will remain there when the board is
                // turned off.
               // EEPROM.write(amin, minVal);
               // EEPROM.write(amax, maxVal);
                EEPROM.write(acurrent, (int)setCurrent);
              
                // advance to the next address.  there are 512 bytes in
                // the EEPROM, so go back to 0 when we hit 512.
                // save all changes to the flash.
               
                  EEPROM.commit();
               
                 delay(300);
                  
               
            
                   //This is not going to be called
                  //Serial.printf("min counter value: %u\n", minVal);
                  //Serial.printf("max counter value: %u\n", maxVal);
                  //Serial.printf("current  value: %u\n", current);
                  //Serial.printf("actual value: %u\n", actualVal);
                  
                  int difference = abs(actualVal - current);
                  //Serial.printf("difference value: %u\n", difference);
                  
                     //Serial.println("send data");
                    
                     
                     //Serial.print("percentage value: ");
                     //Serial.println(percentage);
                     
            
            
               
                    
                    // get time from ntp server
                    timeClient.begin();
                    timeClient.update();
                    delay(200);
                    //ntp_time = timeClient.getEpochTime();
                    //Serial.print("get time ntp - ");
                    //Serial.println(ntp_time);
                    //connect to broker
                    reconnect();
            
            
            
            
            
                    
                  if (!client.connected())
                  {
                    _keepAlive();
                  }
                    if (!client.loop()) {
                    //Serial.print("Client disconnected...");
                   if (client.connect(thing_id.c_str(),"kapua-sys","kapua-password")) {
                    //if (client.connect("b43c-weather-station","kapua-broker","kapua-password")) {
                      //Serial.println("reconnected.");
                    }
                    else {
                      //Serial.println("failed.");
                    }
                  }
            
            
            
            
              
               //timeValue = timeValue +1;
                    delay(1000);
                      // this is our 'wait for incoming subscription packets' busy subloop
                      // try to spend your time here
                     
                             //Serial.println(now());
                             //Serial.println(timeClient.getEpochTime());
                             //Serial.println(unixTime);
                      //unixTime = now() + ntp_time + 7200;
                      int countTime = 0;
                          while (ntp_time<500 && countTime <100)
                          {
                            timeClient.update();
                            ntp_time = timeClient.getEpochTime();
                            countTime++;
                            
                          } 
                          //Serial.print("get time ntp - ");
                            //Serial.println(ntp_time);
                          //Serial.print("countTime - ");
                            //Serial.println(countTime);
                            //Serial.println("################ Encoding Start ###################");
                            //Serial.println();
                              // Create message struct
                              // Create byte buffer for sending out on MQTT
                            uint8_t buffer[1280];
                               
                            kuradatatypes_KuraPayload message = kuradatatypes_KuraPayload_init_zero;
                         
                            
                            display_name.string_value.funcs.encode =  &write_string_display_name_val;
                             
                            model_id.string_value.funcs.encode = &write_string_model_id_val;
                            part_number.string_value.funcs.encode = &write_string_part_number_val;
                            serial_number.string_value.funcs.encode = &write_string_serial_number_val;
                            firmware_version.string_value.funcs.encode = &write_string_firmware_version_val;
                            bios_version.string_value.funcs.encode  = &write_string_bios_version_val;
                            os.string_value.funcs.encode = &write_string_os_val;
                            os_version.string_value.funcs.encode = &write_string_os_version_val;
                            model_name.string_value.funcs.encode = &write_string_model_name_val;
                            
                            display_name.name.funcs.encode = &write_string_display_name; 
                            model_id.name.funcs.encode = &write_string_model_id;
                            part_number.name.funcs.encode = &write_string_part_number;
                            serial_number.name.funcs.encode = &write_string_serial_number;
                            firmware_version.name.funcs.encode = &write_string_firmware_version;
                            bios_version.name.funcs.encode = &write_string_bios_version;
                            os.name.funcs.encode = &write_string_os;
                            os_version.name.funcs.encode = &write_string_os_version;
                            model_name.name.funcs.encode = &write_string_model_name;
                            
                            message.has_position=true;
                            message.position= devicePosition;
            
                          
                            
                            message.metric.funcs.encode = &metric_callback_birthday; 
                             
                            
                            pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
                            
                            
                            
                              
                              // Try to encode
                              if (!pb_encode(&stream, kuradatatypes_KuraPayload_fields, &message)) {
                                
                                //Serial.println(F("Encoding failed"));
                                //Serial.println(PB_GET_ERROR(&stream));
                              }
                              else
                              {// successful encoding birthda message, sending message below
                           
                             
                                client.publish(topic_birthmessage.c_str(),buffer,stream.bytes_written);
                                sentbirthmessage= true;
            
                                //Serial.println("birth message sent");  
            
            
            
                               //Serial.println("encoded message1 - ");
                                /*  for (int i=0; i< stream.bytes_written; i++){
                                    if (buffer[i]<0x10) {Serial.print("0");} 
                                       Serial.print(buffer[i],HEX); 
                                       Serial.print(" "); 
                                    }*/
                                   
                                 //Serial.println(" ");
                                  
                                                     
                                }
                                
                             
                               
                          //################ Encoding End Birthday message ###################");
                   delay(2000);
                       
            
            
                            //Serial.println("################ Encoding DATA Start ###################");
                            //Serial.println();
            
                            //get time
                            
                              // Create message struct
                              // Create byte buffer for sending out on MQTT
                            //uint8_t buffer[1280];
                               
                            //kuradatatypes_KuraPayload message = kuradatatypes_KuraPayload_init_time;
                            
                            int sendPercent = round(percentage);
                            light_metric.int_value =  sendPercent;
                            
            
                         //Serial.print ("sendPercent - ");
                            
                              //Serial.println(sendPercent);
                              
                            char *displayReading; 
                            //printf(displayReading,"temp = %d  hum = %d air = %d lux = %d  ampitude = %d", ccTemp,humdity_i2c  ,CO2  ,ampitude, lux);
                           // Serial.printf(displayReading,"temp = %d  hum = %d air = %d lux = %d  ampitude = %d", ccTemp,humdity_i2c  ,CO2  ,ampitude, lux);
                            light_metric.name.funcs.encode = &write_light; 
                           
                             
                            
                            message.has_timestamp = true;
                           //timeValue = timeValue +1;
                           
                           
                           message.timestamp = (int64_t)ntp_time*1000;
                           timeClient.update();
                            ntp_time = timeClient.getEpochTime();
                            //Serial.print("timestamp - ");
                            char str[21];
                          //Serial.println( uintToStr( ntp_time, str ) );
             
            
            
                            message.metric.funcs.encode = &metric_callback_data;
                            //message.has_timestamp=true;
                            //message.timestamp =  unixTimeL;
                              stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
                               
                              // Try to encode
                              if (!pb_encode(&stream, kuradatatypes_KuraPayload_fields, &message)) {
                                
                                //Serial.println(F("Encoding failed"));
                                //Serial.println(PB_GET_ERROR(&stream));
                              }
                              else
                              {// successful encoding birthda message, sending message below
                           
                             
                                client.publish(topic_data.c_str(),buffer,stream.bytes_written);
                                sentbirthmessage= true;
                                //Serial.println("data sent");  
            
                              digitalWrite(LED,LOW);
                     delay(1000);
                /*digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);*/
                delay(1000);
            
                               //Serial.println("encoded message1 - ");
                                 /* for (int ii=0; ii< stream.bytes_written; ii++){
                                    if (buffer[ii]<0x10) {Serial.print("0");} 
                                       Serial.print(buffer[ii],HEX); 
                                       Serial.print(" "); 
                                    }*/
                                   
                                 //Serial.println(" ");
                                                     
                              }
                                
                             //Serial.println("################ Encoding DATA End ###################");
                 
            
            
                     
                  
            
                
                  
         }
       }else {
              
               if (lowSet){
                   //Serial.println("write   min");
                    //Serial.println("old  val "); 
          
                    //Serial.println(EEPROM.read(amin)*4);
                    delay(200);
                  int low= analogRead(0)/4;
                  uint8_t save = (uint8_t)low;
                   //Serial.println("reading "); 
          
                    //Serial.println(save);
                    EEPROM.write(amin, (int)save);
                    delay(200);
                     EEPROM.commit();
                      delay(900);
                      //Serial.println("updateed min "); 
               
                    //Serial.println(EEPROM.read(amin)*4);
               
                /*digitalWrite(LED,LOW); 
                 delay(1000);   
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);*/
                  }
                 if (highSet){
                      //Serial.println("write    max");
                      //Serial.println(EEPROM.read(amax)*4);
                    //Serial.println(EEPROM.read(amin)*4);
                    delay(200);
                  int high= analogRead(0)/4;
                  uint8_t save = (uint8_t)high;
                 EEPROM.write(amax, (int)save);
                    delay(700);
                     EEPROM.commit();
                      delay(900);
                     
                   //Serial.println(EEPROM.read(amax)*4);   

                /*digitalWrite(LED,LOW); 
                 delay(1000); 
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);
                digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);
                 digitalWrite(LED,HIGH);
                delay(1000);
                digitalWrite(LED,LOW);
                delay(1000);*/
                 }

                  
               
                 delay(600);
        EEPROM.end();
        }
        //Serial.println("Going to sleep now");
                   // ESP.deepSleep(300e6); // 20e6 is 20 microseconds

                     
  WiFiOff();
  delay(5000);
                   delay(200);
             ESP.deepSleep(3600e6);
                    //Serial.println("This will never be printed");
                 
  }






  

bool metric_callback_birthday(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) 
     {
          //Serial.println("metric_callback ()!");
          if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
          // Serial.println(" (!pb_encode_tag_for_field(stream, field))==true");

              //################  display_name
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &display_name)) 
          {
            Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &display_name))==false");return false;
          }
          
           //################  part_number
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          } 
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &part_number)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &part_number))==false");return false;
          }
           

       
          
          //################  model_id

           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &model_id)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &model_id))==false");return false;
          }
          

            //################  model_name

           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &model_name)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &model_name))==false");return false;
          }
               
         
          //################  serial_number
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
           if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &serial_number)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &serial_number))==false");return false;
          }
           
          //################  firmware_version
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
           if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &firmware_version)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &firmware_version))==false");return false;
          }
           
           
          //################  bios_version
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
           if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &bios_version)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &bios_version))==false");return false;
          }
          
          
          //################  os
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
           if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &os)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &os))==false");return false;
          }
          
        
          
          //################  os_version
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
           if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &os_version)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &os_version))==false");return false;
          }
           
                             
      }


   
bool metric_callback_data(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) 
     {
          //Serial.println("metric_callback ()!");
          if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
          // Serial.println(" (!pb_encode_tag_for_field(stream, field))==true");

              //################  Light 
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &light_metric)) 
          {
            Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &display_name))==false");return false;
          }
          /*
           //################  Air Quailty
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          } 
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &air_metric)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &part_number))==false");return false;
          }
           

       
          
          //################  Humidity

           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &humd_metric)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &model_id))==false");return false;
          }
          

            //################  Sound

           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
          if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &sound_metric)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &model_name))==false");return false;
          }
               
         
          //################  Temperature
           if (!pb_encode_tag_for_field(stream, field)) 
          {
            Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;
          }
           if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &temp_metric)) 
          {
             Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &serial_number))==false");return false;
          }
           
          */
   
                             
      }




// for single metric
bool metric_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
       //Serial.println("metric_callback ()!");
     if (!pb_encode_tag_for_field(stream, field)) {Serial.println(" (!pb_encode_tag_for_field(stream, field))==false");return false;}
     if (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &valores)) {Serial.println(" (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &valores))==false");return false;}
  {Serial.println("metric_callback -> (!pb_encode_submessage(stream, kuradatatypes_KuraPayload_KuraMetric_fields, &valores))==true"); return true; }
}

 


//setting the field name for first metric 
bool write_light(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
      
    //char *str = get_string_from_somewhere();
    char *str = "Paper-level-0";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}
 
//setting the field name for first metric 
bool write_hum(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
      
    //char *str = get_string_from_somewhere();
    char *str = "humidity-0";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}

 

//setting the field name for second metric 
bool write_temp(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
      
    //char *str = get_string_from_somewhere();
    char *str = "temperature-0";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}











 
bool write_string_display_name(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "display_name";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)display_name_field, strlen(display_name_field));
}



bool write_string_model_id(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "model_id";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)model_id_field, strlen(model_id_field));
}



bool write_string_part_number(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "part_number";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)part_number_field, strlen(part_number_field));
}

bool write_string_os_version(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "os_version";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)os_version_field, strlen(os_version_field));
}

bool write_string_os(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "os";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)os_field, strlen(os_field));
}

bool write_string_bios_version(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "bios_version";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)bios_version_field, strlen(bios_version_field));
}

bool write_string_firmware_version(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
   // char *str = "firmware_version";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)firmware_version_field, strlen(firmware_version_field));
}

bool write_string_serial_number(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "serial_number";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)serial_number_field, strlen(serial_number_field));
}

bool write_string_model_name(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
   // char *str = "model_name";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)model_name_field, strlen(model_name_field));
}
 






// ##############################################################################################
// ##########################   Call back function for birth message date#####################################
// ##############################################################################################
bool write_string_display_name_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
     
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)display_name_v, strlen(display_name_v));
}



bool write_string_model_id_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "123";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)model_id_v, strlen(model_id_v));
}


bool write_string_part_number_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "01";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)part_number_v, strlen(part_number_v));
}

bool write_string_os_version_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "3.10.96-114";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)os_version_v, strlen(os_version_v));
}

bool write_string_os_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "Linux";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)os_v, strlen(os_v));
}

bool write_string_bios_version_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "1.0";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)bios_version_v, strlen(bios_version_v));
}
 
bool write_string_model_name_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "kapua-sys/b4311-142-2a77-kettle/type.water.heating.kettle/power-w/b4311-142-2a77-kettle_circuit_1_power, kapua-sys/b4311-142-2a77-kettle/type.water.heating.kettle/actuator-on-off/b4311-142-2a77-kettle_circuit_1_switch-auto-state";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)model_name_v, strlen(model_name_v));
}

bool write_string_firmware_version_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "1.0";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)firmware_version_v, strlen(firmware_version_v));
}

bool write_string_serial_number_val(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{ 
    //char *str = "100";
    if (!pb_encode_tag_for_field(stream, field))
        {Serial.println("write_string ->  (!pb_encode_tag_for_field(stream, field))==false");return false;}
    
    return pb_encode_string(stream, (uint8_t*)serial_number_v, strlen(serial_number_v));
}

// ##############################################################################################







 





//  received mqtt messages


  void callback(char* topic, byte* payload, unsigned int length) {
   
  //Serial.print("Message arrived topic [");
 
  //Serial.print(topic);
   //Serial.print(" ] lenght = ");
     //Serial.print(length);
  //Serial.print(" message = [");
  for (int i = 0; i < length; i++) {
     if (payload[i]<0x10) {Serial.print("0");} 
    //Serial.print( payload[i],HEX);
    gotPayload[i] = (uint8_t)payload[i];
    //Serial.print(" ");
  }
  //Serial.println("]");
  //Serial.println("message above");
 

  //gotMessage = true;

 //Serial.println("########## begin Decoding###########");
      //SimpleMessage message = SimpleMessage_init_zero;
      kuradatatypes_KuraPayload messagedecode = kuradatatypes_KuraPayload_init_default;  
        // Create a stream that reads from the buffer. /
        //pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
         pb_istream_t inputStream = pb_istream_from_buffer(gotPayload, gotPayloadLength);
         //Serial.println("broke 1");
        // Now we are ready to decode the message. /





        messagedecode.metric.funcs.decode = &print_int32;
        //messagedecode.metric.funcs.decode =&print_string;
        
         char *testing5 = "stringvalue: \"%s\"\n";
        messagedecode.metric.arg = testing5;

        metricBufferCount = 0;
        metricCount = -1;
        bool status = pb_decode(&inputStream, kuradatatypes_KuraPayload_fields, &messagedecode);
        // Check for errors... 
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&inputStream));
            //Serial.println();Serial.println("pb_decode(&inputStream, kuradatatypes_KuraPayload_fields, &messagedecode) Decoding as FAILED!! ###########");
          
        }else{
            //Serial.print("messagedecode.timestamp ");
            //Serial.println((long long)messagedecode.timestamp);
            char str[21];
             //Serial.println( uintToStr( messagedecode.timestamp, str ) );
             pb_istream_t metricStream = pb_istream_from_buffer(metricBuffer, metricBufferCount);  
          
              decodem.name.funcs.decode = &print_string;
               

            status = pb_decode(&metricStream, kuradatatypes_KuraPayload_KuraMetric_fields, &decodem);
            // Check for errors... 
            if (!status){
              printf("Decoding failed: %s\n", PB_GET_ERROR(&inputStream));
            //Serial.println();Serial.println("pb_decode(&inputStream, kuradatatypes_KuraPayload_fields, &messagedecode) Decoding as FAILED!! ###########");
            
            }else{
             
            //Serial.print(decodem.has_double_value);
            //Serial.print(" ");
             //Serial.print(decodem.double_value);
            //Serial.print(" ");
             //Serial.print(decodem.has_float_value);
            //Serial.print(" ");
             //Serial.print(decodem.float_value);
            //Serial.print(" ");
             //Serial.print(decodem.has_long_value);
            //Serial.print(" ");
            //Serial.print((int)decodem.long_value);
            //Serial.print(" ");
             //Serial.print(decodem.has_int_value);
            //Serial.print(" ");
             //Serial.print(decodem.int_value);
            //Serial.print(" ");
        
             //Serial.print(decodem.has_bool_value);
            //Serial.print(" ");
             //Serial.print(decodem.bool_value);
            //Serial.print(" c ");
            //Serial.print(metricCount);
            metricCount = metricCount +1;
            metricArryI[metricCount] = decodem.int_value;
       
            //Serial.println(" start array"); 
            for (int j = 0 ; j<= metricCount; j++){
              //Serial.print(metricArryS[j]);
               //Serial.print(" "); 
                 //Serial.print(metricArryI[j+1]);
              }   
            }


 
          callbackcount = 0;
          pb_callback_t tester = messagedecode.metric;
          
         
       
   
    
    
        // ******** End decodeing the Protobuf  *******************/

      decoded = true;
      //Serial.println("########## END Decoding###########");


    }



   
  
  gotPayloadLength = length;
  
 
}



bool print_int32(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    //Serial.printf((char*)*arg, (long)value);
   //  Serial.print("  ");
        //  Serial.println( (int)value);
         
          metricBuffer[metricBufferCount] = (int)value;
            metricBufferCount = metricBufferCount +1;
    return true;
}


bool print_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    if (stream->bytes_left > sizeof(buffer) - 1)
       {return false;}
    
    if (!pb_read(stream, buffer, stream->bytes_left))
        {return false;}
    
    /* Print the string, in format comparable with protoc --decode.
     * Format comes from the arg defined in main().
     */
    //Serial.print((char*) buffer);

   //Serial.println();


       
   /* Serial.print(decodem.has_double_value);
    Serial.print(" ");
     Serial.print(decodem.double_value);
    Serial.print(" ");
     Serial.print(decodem.has_float_value);
    Serial.print(" ");
     Serial.print(decodem.float_value);
    Serial.print(" ");
     Serial.print(decodem.has_long_value);
    Serial.print(" ");
    Serial.print((int)decodem.long_value);
    Serial.print(" ");
     Serial.print(decodem.has_int_value);
    Serial.print(" ");
     Serial.print(decodem.int_value);
    Serial.print(" ");

     Serial.print(decodem.has_bool_value);
    Serial.print(" ");
     Serial.print(decodem.bool_value);
    Serial.print(" c = ");
    Serial.print(metricCount);*/
    metricCount = metricCount +1;
    metricArryI[metricCount] = decodem.int_value;
    metricArryS[metricCount] = (char*) buffer;

    
    return true;
}



void WiFiOn() {

  wifi_fpm_do_wakeup();
  wifi_fpm_close();

  //Serial.println("Reconnecting");
  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();
}


void WiFiOff() {

  //Serial.println("diconnecting client and wifi");
  //client.disconnect();
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);

}








