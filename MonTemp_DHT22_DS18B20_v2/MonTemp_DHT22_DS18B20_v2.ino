
#include <OneWire.h>
#include <stdio.h>
#include <math.h>

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <DHT.h>
#include <pins_arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

DHT dht(DHTPIN, DHTTYPE);
OneWire  ds(7);
byte ds18b20_line_ofs=0;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


/* Global Varibles */
byte count_ds18b20=0;

byte discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  byte count = 0;
 
  Serial.println("Looking for 1-Wire devices...");
  while(ds.search(addr)) {
    Serial.print("\'1-Wire\' device : ");
    for( i = 0; i < 8; i++) {
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(" ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return 0;
    }
    Serial.println();
    count++;
  }
  Serial.println("That's it.");
  ds.reset_search();
  return count;
}


void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();
  Serial.begin(9600);
  
  count_ds18b20=discoverOneWireDevices();
  
  Serial.println("DHT_PIN = %d",(int)DHTPIN);
  Serial.print("num_ds18b20  =  %d",(int)count_ds18b20);
  

  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, DHT LCD");
  
  lcd.setCursor(3,1);
  lcd.print(count_ds18b20);


}

void loop() {

} 

