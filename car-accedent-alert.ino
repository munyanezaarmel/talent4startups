#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <MPU6500_WE.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define MPU6500_ADDR 0x68

ESP8266WiFiMulti WiFiMulti;

int id = 0;
int sensor1 = 0;
int sensor2 = 0;
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);
const float ACCEL_THRESHOLD = 4.0 * 9.81; // 2 g (m/s^2)
const float GYRO_THRESHOLD = 50.0; // Adjust based on testing (degrees/s)
unsigned long alarm_duration = 2000; // Alarm duration in milliseconds

#define RX_PIN 3  module
#define TX_PIN 2  

TinyGPSPlus gps; // the TinyGPS++ object
SoftwareSerial gpsSerial(RX_PIN, TX_PIN); // the serial interface to the GPS module

void setup() {
  gpsSerial.begin(9600);  
  Serial.begin(115200);

   Wire.begin();

  //call acceleromete setups
  accelerometer_setup();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(200);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Armel", "Armel@(1%){_2001?*}");
}

void loop() {
    gps_module();
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    
    id = 1;
    sensor1 = random(1, 100);
    sensor2 = random(1, 100);
    
    String url = "http://api.thingspeak.com/update?api_key=DUSGLM0B9IZ9IRGB";
    url += "&field1=" + String(id);
    url += "&field2=" + String(sensor1);
    url += "&field3=" + String(sensor2);

    if (http.begin(client, url)) {  // HTTP request
      Serial.print("[HTTP] GET...\n");
      
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been sent and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  }

  delay(10000);  // wait 10 seconds before sending the next data
}

void accelerometer_setup(){
  if(!myMPU6500.init()){
    Serial.println("MPU6500 does not respond");
  }
  else{
    Serial.println("MPU6500 is connected");
  }
  
  Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
  delay(1000);
  myMPU6500.autoOffsets();
  Serial.println("Done!");

  // enabl the digital low pass filter (DLPF).
  myMPU6500.enableGyrDLPF();
  myMPU6500.setGyrDLPF(MPU6500_DLPF_6);

  /*  Sample rate divider divides the output rate of the gyroscope and accelerometer.
   *  Sample rate = Internal sample rate / (1 + divider) 
   *  It can only be applied if the corresponding DLPF is enabled and 0<DLPF<7!
   *  Divider is a number 0...255
   */
  myMPU6500.setSampleRateDivider(5);

  /*  MPU6500_GYRO_RANGE_250       250 degrees per second (default)
   */
  myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);

  /*  MPU6500_ACC_RANGE_2G      2 g   (default)
   *  MPU6500_ACC_RANGE_4G      4 g
   *  MPU6500_ACC_RANGE_8G      8 g   
   *  MPU6500_ACC_RANGE_16G    16 g
   */
  myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);

  /*  Enable/disable the digital low pass filter for the accelerometer 
   *  If disabled the bandwidth is 1.13 kHz, delay is 0.75 ms, output rate is 4 kHz
   */
  myMPU6500.enableAccDLPF(true);

  /*  Digital low pass filter (DLPF) for the accelerometer, if enabled 
   *  MPU6500_DPLF_0, MPU6500_DPLF_2, ...... MPU6500_DPLF_7 
   *   DLPF     Bandwidth [Hz]      Delay [ms]    Output rate [kHz]
   *     0           460               1.94           1
   *     1           184               5.80           1
   *     2            92               7.80           1
   *     3            41              11.80           1
   *     4            20              19.80           1
   *     5            10              35.70           1
   *     6             5              66.96           1
   *     7           460               1.94           1
   */
  myMPU6500.setAccDLPF(MPU6500_DLPF_6);
  delay(200);
}

void accelerometer(){
  xyzFloat gValue = myMPU6500.getGValues();
  xyzFloat gyr = myMPU6500.getGyrValues();
  float temp = myMPU6500.getTemperature();
  float resultantG = myMPU6500.getResultantG(gValue);

  Serial.println("Acceleration in g (x,y,z):");
  Serial.print(gValue.x);
  Serial.print("   ");
  Serial.print(gValue.y);
  Serial.print("   ");
  Serial.println(gValue.z);
  Serial.print("Resultant g: ");
  Serial.println(resultantG);

  Serial.println("Gyroscope data in degrees/s: ");
  Serial.print(gyr.x);
  Serial.print("   ");
  Serial.print(gyr.y);
  Serial.print("   ");
  Serial.println(gyr.z);

  Serial.print("Temperature in °C: ");
  Serial.println(temp);

  // Calculate total acceleration magnitude
  float total_accel = sqrt(pow(gValue.x, 2) + pow(gValue.y, 2) + pow(gValue.z, 2));
  Serial.println("total accel:"+String(total_accel));

  // Check for accident conditions
  if (total_accel > ACCEL_THRESHOLD || abs(gyr.x) > GYRO_THRESHOLD || abs(gyr.y) > GYRO_THRESHOLD || abs(gyr.z) > GYRO_THRESHOLD) {
    unsigned long start_time = millis();

    // Trigger alarm
    Serial.println("Accident detected!");
    // while (millis() - start_time < alarm_duration) {
    //   Serial.println("Accident detected!");
    //   digitalWrite(ledPin, HIGH); // Turn on LED (if using)
    //   // Add your GSM module code here (if applicable)
    // }
    //digitalWrite(ledPin, LOW); // Turn off LED (if using)
  }
}

void gps_module(){
  if (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        Serial.print(F("- latitude: "));
        Serial.println(gps.location.lat());

        Serial.print(F("- longitude: "));
        Serial.println(gps.location.lng());

        Serial.print(F("- altitude: "));
        if (gps.altitude.isValid())
          Serial.println(gps.altitude.meters());
        else
          Serial.println(F("INVALID"));
      } else {
        Serial.println(F("- location: INVALID"));
      }

      Serial.print(F("- speed: "));
      if (gps.speed.isValid()) {
        Serial.print(gps.speed.kmph());
        Serial.println(F(" km/h"));
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.print(F("- GPS date&time: "));
      if (gps.date.isValid() && gps.time.isValid()) {
        Serial.print(gps.date.year());
        Serial.print(F("-"));
        Serial.print(gps.date.month());
        Serial.print(F("-"));
        Serial.print(gps.date.day());
        Serial.print(F(" "));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        Serial.println(gps.time.second());
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.println();
      //accelerometer looping
      accelerometer();

      Serial.println("****************");
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10){
    Serial.println(F("No GPS data received: check wiring"));
  }
}
