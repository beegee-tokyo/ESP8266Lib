#ifndef ntp_lib_h
#define ntp_lib_h

#include <TimeLib.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

bool tryGetTime(boolean debugOn = false);
time_t getNtpTime();
void sendNTPpacket(WiFiUDP udpNTP);
String digitalClockDisplay();
String getDigits(int digits);

/** Flag if getting the time from NTP was successful */
extern bool gotTime;

#endif // ntp_lib_h
