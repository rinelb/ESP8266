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
  int acurrent = 12;
  
  int minv =  0;
  int maxv =  0;
  int currentv =  0;
  
  
  void setup()
  {
    Serial.begin(115200);
  Serial.setTimeout(2000);
    EEPROM.begin(32);
  }
  
  void loop()
  {
     
    Serial.println("I'm awake.");
    // need to divide by 4 because analog inputs range from
    // 0 to 1023 and each byte of the EEPROM can only hold a
    // value from 0 to 255.
    minv = EEPROM.read(amin);
     maxv = EEPROM.read(amax);
     currentv = EEPROM.read(acurrent);
     Serial.println("Values");
     Serial.println(minv);
     Serial.println(maxv);
     Serial.println(currentv);
     
        minv +=  1;
    maxv +=  2;
    currentv +=  5;
    // write the value to the appropriate byte of the EEPROM.
    // these values will remain there when the board is
    // turned off.
    EEPROM.write(amin, minv);
    EEPROM.write(amax, maxv);
    EEPROM.write(acurrent, currentv);
  
    // advance to the next address.  there are 512 bytes in
    // the EEPROM, so go back to 0 when we hit 512.
    // save all changes to the flash.
   
      EEPROM.commit();
   
    delay(100);
     Serial.println("Going into deep sleep for 20 seconds");
     ESP.deepSleep(20e6); // 20e6 is 20 microseconds
     
  }
