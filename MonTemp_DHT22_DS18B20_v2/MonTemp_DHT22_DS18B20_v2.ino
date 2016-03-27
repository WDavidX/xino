/*
 * This program allows Arduino to monitor temperature and humidity
 * One DHT22 humidity and temperature sensor is connected PIN 2
 * Several DS18B20 temperature sensors are connected to PIN 7 OneWire bus
 * The measurement would be displayed on an LCD on I2C bus addr 0x27
 * Measurement is also sent out through serial port at BR 11520
 * Each line in the serial port contains one sweep from all sensors
 * The first two numbers are the temperature in Celcius and humidity
 * The following numbers are the DS18B20 readings.
 */

#include <stdio.h>
#include <math.h>

#include <OneWire.h>  // One wire bus basic library
#include <DallasTemperature.h>  // DS18B20 high-level library
#include <DHT.h>    //DHT22 temperature/humidity sensor
//#include <pins_arduino.h> // Turn on-board LED off
#include <LiquidCrystal_I2C.h> //LCD in I2C bys

/* Define macros */
// Sensor buses
#define OneWire_Bus_PIN 7 
#define DHT_PIN 2        // signal pin connected to the DHT
#define DHT_TYPE DHT22   // DHT 22  (AM2302)
// LCD parameters
#define DISPLAY_W 20     
#define DISPLAY_H 4     
//#define DISPLAY_I2CADDR 0x27 
#define DISPLAY_I2CADDR 0x3f
#define DISPLAY_LINE_STARTPOS 3

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


/* Global Objects */
LiquidCrystal_I2C lcd(DISPLAY_I2CADDR,DISPLAY_W,DISPLAY_H);  // set the LCD address to 0x27 on I2C
DHT dht(DHT_PIN, DHT_TYPE);           // DHT instance
OneWire  oneWire(OneWire_Bus_PIN);    // OneWire bus instance
DallasTemperature sensors(&oneWire);  // DS18B20 wrapper on OneWire bus


/* Global Varibles */
float tempC_dht=0,humidity_dht=0,tempF_dht=0;   // For the DHT
float tempC_sensor=0,tempF_sensor=0;            // For DS18B20
int count_ds18b20;

// LCD related settings
char lcd_line[DISPLAY_W+1];                     // String to be displayed
char temp_str[DISPLAY_W+1];                     // hold formatted numbers
uint8_t line_cursor_pos=0;
boolean is_even_loop=true;


void setup() {
  // Init DS18B20 on OneWire bus
  sensors.begin();
  count_ds18b20=sensors.getDeviceCount() ;

  // Init LCD, DHT and the serial port
  lcd.init();
  lcd.backlight();
  dht.begin();
  Serial.begin(115200);

  // Display 
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Hello Alli Station");

  lcd.setCursor(3,1);
  lcd.print("DHT PIN at  ");
  lcd.print(DHT_PIN);

  lcd.setCursor(3,2);
  lcd.print("OneWire at  ");
  lcd.print(OneWire_Bus_PIN);

  lcd.setCursor(3,3);
  lcd.print("Sensors     ");
  lcd.print(count_ds18b20);

  delay(2000);
  lcd.noBacklight();
  lcd.clear();
  //TXLED0;
  //RXLED0;
}



void loop() {

  int i;
  uint8_t lcd_line_pos;  

  /* Read DHT 22 sensors */

  tempC_dht=dht.readTemperature();
  humidity_dht = dht.readHumidity();
  tempF_dht=tempC_dht*1.8+32;
  if (!(isnan(humidity_dht) || isnan(tempC_dht) )) {
    //display DHT readings
    tempF_dht=tempC_dht*1.8+32;
  }
  
  //Serial output
  Serial.print(tempC_dht);
  Serial.print(" \t");
  Serial.print(humidity_dht);
  Serial.print(" \t\t\t");
  
  // Display on the LCD

  line_cursor_pos=0;
  dtostrf(tempF_dht,5,1,temp_str);
  strcpy(&lcd_line[line_cursor_pos],temp_str);
  
  line_cursor_pos+=strlen(temp_str);
  strcpy(&lcd_line[line_cursor_pos],"F ");
  
  line_cursor_pos+=2;
  dtostrf(tempC_dht,5,1,temp_str);
  strcpy(&lcd_line[line_cursor_pos],temp_str);    
  
  line_cursor_pos+=strlen(temp_str);
  strcpy(&lcd_line[line_cursor_pos],"C "); 

  line_cursor_pos+=2;
  dtostrf(humidity_dht,3,0,temp_str);
  strcpy(&lcd_line[line_cursor_pos],temp_str);   

  line_cursor_pos+=strlen(temp_str);
  strcpy(&lcd_line[line_cursor_pos],"%"); 

  lcd.setCursor(1,0);
  lcd.print(lcd_line);

  // LCD heart beat
  lcd.setCursor(0,0);
  if (is_even_loop) {
    lcd.print("*"); 
  }else{
    lcd.print(" ");
  }
  is_even_loop=!(is_even_loop);
  
  /* Read DS18B20 sensors */
 
  sensors.requestTemperatures(); // Send the command to get temperatures

  for (i=0;i<count_ds18b20;i++)
  { 

    if ( i>(DISPLAY_H-1) && i%(DISPLAY_H-1)==0)
      delay(1000);
    
    tempC_sensor=sensors.getTempCByIndex(i);
    tempF_sensor=tempC_sensor*1.8+32;
    
    // Serial output
    Serial.print(tempC_sensor,4);
    Serial.print("  ");
    
    // LCD Display    
    line_cursor_pos=0;
    dtostrf(tempC_sensor,5,1,temp_str);
    strcpy(&lcd_line[line_cursor_pos],temp_str);
    
    line_cursor_pos+=strlen(temp_str);
    strcpy(&lcd_line[line_cursor_pos],"C  ");
    
    line_cursor_pos+=3;
    dtostrf(tempF_sensor,5,1,temp_str);
    strcpy(&lcd_line[line_cursor_pos],temp_str);    
    
    line_cursor_pos+=strlen(temp_str);
    strcpy(&lcd_line[line_cursor_pos],"F"); 

    lcd.setCursor(DISPLAY_LINE_STARTPOS,1+i%count_ds18b20);
    lcd.print(lcd_line);    
  }
    Serial.println();

} 




/* Reference */
// https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/DallasTemperature.h

/* Hardware configuration */
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

