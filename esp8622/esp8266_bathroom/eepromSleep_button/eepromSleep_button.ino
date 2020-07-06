/*
   * EEPROM Write
   *
   * Stores values read from analog input 0 into the EEPROM.
   * These values will stay in the EEPROM when the board is
   * turned off and may be retrieved later by another sketch.
   */
  
  #include <EEPROM.h>
  
  // the current address in the EEPROM (i.e. which byte
  // we're going to write to next)
  int amin = 4;
  int amax = 8;
  int acurrent = 20;

  int LED =2;
  
int actualVal;
 int minVal;
  int maxVal ;
  int setCurrent; 
  int current;
  int inhigh = 25;
  int inlow = 13;
  
  void setup()
  {
    Serial.begin(115200);
     pinMode(inhigh,INPUT);
        pinMode(LED,OUTPUT);
     pinMode(inlow,INPUT); 
  Serial.setTimeout(2000);
    EEPROM.begin(320);
  }
  
  void loop()
  {


 //blink led

    digitalWrite(LED,HIGH);
    delay(1000);
    digitalWrite(LED,LOW);
    delay(1000);
    digitalWrite(LED,HIGH);
    delay(1000);
    digitalWrite(LED,LOW);
    delay(1000);
    
    if (digitalRead(inlow)==HIGH){
    
      int low= analogRead(0)/4;
        EEPROM.write(amin, low);
    
      }else{minVal = EEPROM.read(amin)*4;}
     if (digitalRead(inhigh)==HIGH){
        
      int high= analogRead(0)/4;
     
     EEPROM.write(amax, high);
      
     }else{maxVal = EEPROM.read(amax)*4;}
     
    Serial.println("I'm awake.");
    actualVal = analogRead(0);
    Serial.println("I'm awake.");
    // need to divide by 4 because analog inputs range from
    // 0 to 1023 and each byte of the EEPROM can only hold a
    // value from 0 to 255. 
     
     current = EEPROM.read(acurrent)*4;
      
        minVal  =  1000;
   
    setCurrent = actualVal/4 ;
    Serial.printf("setCurrent  value: %u\n", setCurrent);
     
     
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
     Serial.println("Going into deep sleep for 20 seconds");
    

       //This is not going to be called
      Serial.printf("min counter value: %u\n", minVal);
      Serial.printf("max counter value: %u\n", maxVal);
      Serial.printf("current  value: %u\n", current);
      Serial.printf("actual value: %u\n", actualVal);
      
      int difference = abs(actualVal - current);
      Serial.printf("difference value: %u\n", difference);
       if  (difference > 5 )
       {
         Serial.println("send data");
         int numrator = actualVal - minVal;
         int denoinator = maxVal - minVal;
         double percentage = (double)numrator/(double)denoinator*100;
         
         Serial.print("percentage value: ");
         Serial.println(percentage);
         percentage *=100;
       }
       Serial.println("Going to sleep now");
        ESP.deepSleep(20e6); // 20e6 is 20 microseconds

        Serial.println("This will never be printed");
     
  }
