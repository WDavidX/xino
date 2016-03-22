#include <OneWire.h>
#include <stdio.h>
#include <math.h>

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include "pins_arduino.h"
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

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial.print("---- DHT");
  Serial.print(DHTTYPE);
  Serial.print(" test at PIN ");
  Serial.println(DHTPIN);
  dht.begin();

  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, DHT LCD");

}

void loop() {
  // for DS18B20
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  char lcd_line[21];
  char lcd_temp[21];
  byte lcd_line_pos;
  
  float celsius, fahrenheit;
  if ( !ds.search(addr)) {
    //Serial.println("No more DS18B20 addresses.");
    ds.reset_search();
    delay(50);
    return;
  }else{
    for ( i = 0; i < 8; i++) {           // we need 9 bytes
      Serial.print(addr[i], HEX);
      Serial.print(" ");
    }  
      Serial.print(" \t ");      
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  delay(800); 
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  

  
  // Wait a few seconds between measurements.
  //delay(0);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float f = dht.readTemperature(true);
  float f = t*9/5+32;


  // print ds18b20
  Serial.print(celsius,2 );
  Serial.print("  ");

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(h);
    Serial.println(t);
    Serial.println(f);
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(t);
  Serial.print("  ");
  Serial.print(h);
  Serial.print(" \t\t");  
  Serial.print("H: ");
  Serial.print(h,1);
  Serial.print(" %\t");
  Serial.print("T: ");
  Serial.print(t,2);
  Serial.print(" C (");
  Serial.print(f,1);
  Serial.print(" F)  \t\t");
  Serial.print("HeatIndex: ");
  Serial.print(hic,1);
  Serial.print(" C (");
  Serial.print(hif,1);
  Serial.println(" F)");

  // LCD
  lcd.setCursor(0,1);  

  lcd_line_pos=0;
  dtostrf(f,5,1,lcd_temp);
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp);

  strcpy(lcd_temp, "F ");
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp);  

  dtostrf(t,5,1,lcd_temp);
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp);

  strcpy(lcd_temp, "C ");
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp);

  dtostrf(h,3,0,lcd_temp);
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp);  

  strcpy(lcd_temp, "%");
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp);  

  lcd.print(lcd_line);

  // LCD line 2 or 3
  lcd.setCursor(6,2+((ds18b20_line_ofs++)%2));  lcd_line_pos=0;
  dtostrf(celsius,6,2,lcd_temp);
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp); 
  
  strcpy(lcd_temp, "C ");
  strcpy(&lcd_line[lcd_line_pos],lcd_temp);
  lcd_line_pos+=strlen(lcd_temp);
  lcd.print(lcd_line);
} 
