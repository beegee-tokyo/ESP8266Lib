#ifndef wifi_lib_h
#define wifi_lib_h

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ledLib.h>

/** Defines */
/** Connection status */
#define CON_INIT   0 // connection initialized
#define CON_START  1 // connecting
#define CON_GOTIP	 2 // connected with IP
#define CON_LOST   3 // disconnected

/** Variables */
extern byte connStatus;
extern bool usePrimAP;
extern unsigned long wifiConnectStart;
extern bool canSwitchAP;

/** Functions */
void connectInit(); // Initiates connection to WiFi
void connectWiFi(); // Connects to a WiFi network
void initOTA(); // Initializes OTA
bool scanWiFi(); // Scan for available networks
void checkWiFiStatus(); // Check current WiFi connection status

#endif // wifi_lib_h
