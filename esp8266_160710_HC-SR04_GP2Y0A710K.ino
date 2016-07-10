#include <ESP8266WiFi.h>
#include "esp8266_160602_udpTxToLogger.h"

extern "C" {
#include "user_interface.h"
}

/*
 * v0.3 2016 Jul. 10
 *   - send data to udpLogger
 *     + modify loop()
 *     + add sendToUdpLogger();
 *     + use [esp8266_160602_udpTxToLogger]
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
  Serial.begin(115200);
  Serial.println("");

  pinMode(kTrigPin, OUTPUT);
  pinMode(kEchoPin, INPUT);

  // UDP
  WiFi_setup();
  WiFi_printConnectionInfo();
}

void sendToUdpLogger(float dst_cm, float voltage)
{
  char szbuf[100];
  int pos = 0;
  int whl, frac; // whole and fractional parts

  // 1. distance
  whl = (int)dst_cm;
  frac = (int)(dst_cm * 100) % 100;
  pos = sprintf(&szbuf[pos],",dst(cm)=,%d.%02d", whl, frac);

  // 2. voltage
  whl = (int)voltage;
  frac = (int)(voltage * 100) % 100;
  pos = sprintf(&szbuf[pos],",voltage(v)=,%d.%02d\r\n", whl, frac);
 
  WiFi_txMessage(szbuf);    
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

  sendToUdpLogger(dst_cm, voltage);
    
  delay(300); // msec

//  delay(1000); // msec
}
