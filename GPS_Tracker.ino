/* 
 This Sketch uses the TinyGPS library and SD card library to log GPS data and save it on a SD card.
 
 Important: This version is intended for Arduino 1.0 IDE. It will
 not compile in earlier versions. Be sure the following files are
 present in the folder with this sketch:
 
 TinyGPS.h
 TinyGPS.cpp
 keywords.txt
 
 A revised version of the TinyGPS object library is included in the sketch folder
 to avoid conflict with any earlier version you may have in the Arduino libraries 
 location. 
 */

#include <SoftwareSerial.h>
#include "TinyGPS.h"                 // Special version for 1.0
#include <SD.h>                        // Standard Arduino SD card library
#include <LiquidCrystal.h>

File myFile;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

TinyGPS gps;
SoftwareSerial nss(2, 255);            // Yellow wire to pin 2. (Based on using 
                                       // Parallax 28500-RT PMB-648 GPS SiRF Internal Antenna)
                                       
// define some values used by the panel
int lcd_key     = 0;
int adc_key_in  = 0;

void setup() {
  Serial.begin(115200);
  nss.begin(4800);
  Serial.println("Reading GPS");
  
// Initialize LCD Screen
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
// End initialize LCD Screen
  
// Initialize SD card
 lcd.print("Initializing SD");
 lcd.setCursor(0,1);
 Serial.print("Initializing SD card..."); 
   pinMode(10, OUTPUT);
   
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  lcd.print("Initialized");
  lcd.setCursor(0,1);
  Serial.println("initialization done.");
// End initialize SD Card
}

void loop() {
  bool newdata = false;
  unsigned long start = millis();
  while (millis() - start < 5000) {  // Update every 5 seconds
    if (feedgps())
      newdata = true;
  }
  if (newdata) {
    gpsdump(gps);
    
  }
}

// Get and process GPS data
void gpsdump(TinyGPS &gps) {
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  lcd.setCursor(0,0);
  lcd.print(flat, 4);
  Serial.print(flat, 4);
  Serial.print(", ");
  lcd.setCursor(0,1);
  lcd.print(flon, 4);
  Serial.println(flon, 4);
  
   /// And write it to SD card
       myFile = SD.open("tracking.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to tracking.txt...");
    myFile.print(flat, 4);
    myFile.print(", ");
    myFile.println(flon, 4);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening tracking.txt");
  }
    ///  
  
}

// Feed data as it becomes available 
bool feedgps() {
  while (nss.available()) {
    if (gps.encode(nss.read()))
      return true;
      
  }
  return false;
}
