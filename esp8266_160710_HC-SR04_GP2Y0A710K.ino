#include <ESP8266WiFi.h>

extern "C" {
#include "user_interface.h"
}

/*
 * v0.2 2016 Jul. 10
 *   - add ADC for GP2Y0A710K
 * v0.1 2016 Jul. 09
 *   - calculate distance in [cm]
 *   - receive echo in loop() 
 *   - send trigger in loop()
 *   - set pinMode() at setup()
 */

static const int kTrigPin = 12;
static const int kEchoPin = 13;

void setup() {
  WiFi.disconnect(); // avoid watchdog reset by WiFi thread

  Serial.begin(115200);
  Serial.println("");

  pinMode(kTrigPin, OUTPUT);
  pinMode(kEchoPin, INPUT);
}

void loop() {
  // trigger
  digitalWrite(kTrigPin, HIGH);
  delay(100); // msec
  digitalWrite(kTrigPin, LOW);

  // echo
  int interval_us;
  interval_us = pulseIn(kEchoPin, HIGH) / 2; // 2: round-trip
  
//  Serial.print(interval_us);
//  Serial.print(", ");
  
  float dst_m = (float)interval_us * 340.0 / 1000.0 / 1000.0;
  float dst_cm = dst_m * 100.0;

  uint ADvalue;
  ADvalue = system_adc_read();
  float voltage = ADvalue * 1.0 / 1024;

  Serial.print(dst_cm);
  Serial.print("\t");
  Serial.print(voltage);
  Serial.println();
    
  delay(300); // msec

//  delay(1000); // msec
}
