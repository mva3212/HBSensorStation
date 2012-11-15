//HarvestBot SensorStation
//Author - Mike Alt
//Last Edited - 10/27/12
//Modified from Jeelabs examples http://github.com/jcw/ www.jeelabs.org
// and openenergymonitor http://github.com/openenergymonitor/  www.openenergymonitor.org

// ---------- Includes --------------
#include <JeeLib.h>  //https://github.com/jcw/jeelib
#include <avr/wdt.h>
#include <DHT.h>  //https://github.com/adafruit/DHT-sensor-library

//-----------Constants --------------
#define freq RF12_433MHZ
const int nodeID = 10;
const int networkGroup = 88;
const int UNO = 1;      // Set to 0 if your not using the UNO
const int DEBUG = 1;      // Set to 0 if you don't want to serial.print
#define DHTPIN 4     // connected to Port1 ( pin 4)
#define DHTTYPE DHT11   // DHT 11

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor


typedef struct { int temp, humi, light;} SensorStationPayload;


//------------- Declarations ---------------
ISR(WDT_vect) { Sleepy::watchdogEvent(); }
PortI2C myBus (1); // Set up port1 for i2c
LuxPlug sensor (myBus, 0x29);
byte highGain;
DHT dht(DHTPIN, DHTTYPE);
SensorStationPayload ssp;

void setup() {

  if(DEBUG){
      Serial.begin(57600);
      Serial.println("SensorStation Initializing");
    }

  //Start Sensors
  dht.begin();
  highGain = 1;

  rf12_initialize(nodeID, freq, networkGroup);  // initialize RF
  rf12_sleep(RF12_SLEEP); //Go To Sleep
  if (UNO) wdt_enable(WDTO_8S);
}

void loop()
{
  if (UNO) wdt_reset();

  // Temp and Humidity Reading
  ssp.humi = dht.readHumidity() *100;
  ssp.temp = dht.readTemperature() * 100;

  // LUX plug reading
  sensor.begin();
  sensor.setGain(highGain);
  delay(1000); // Wait for proper powerup.
  const word* photoDiodes = sensor.getData();
  ssp.light = sensor.calcLux();
  sensor.poweroff(); // Power off when we've got the data.

  if(DEBUG) print_to_serial();
  send_rf_data();
  sleepForSeconds(10);
}

