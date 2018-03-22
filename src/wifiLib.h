#ifndef wifi_lib_h
#define wifi_lib_h

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include <ledLib.h>
#include <spiffsLib.h>

/** Defines */
/** Connection status */
#define CON_INIT   0 // connection initialized
#define CON_START  1 // connecting
#define CON_GOTIP	 2 // connected with IP
#define CON_LOST   3 // disconnected

/** Global Variables */
extern byte connStatus;
extern bool usePrimAP;
extern unsigned long wifiConnectStart;
extern bool canSwitchAP;
extern WiFiUDP udpListener;
extern int udpBcPort;
extern int tcpComPort;
extern int tcpDebugPort;
extern int tcpRpiDebugPort;
extern bool wdtEnabled;
extern IPAddress multiIP;
extern IPAddress ipMonitor;
extern IPAddress ipCam1;
extern IPAddress ipCam2;
extern IPAddress ipDebug;

extern String mhcIdTag;
extern String secDevice;

/** Functions */
void connectInit(); // Initiates connection to WiFi
void connectWiFi(); // Connects to a WiFi network
void initOTA(char hostApName[]); // Initializes OTA
bool scanWiFi(); // Scan for available networks
void checkWiFiStatus(); // Check current WiFi connection status
void sendDebug(String debugMsg, String senderID); // Send debug message over TCP
void sendRpiDebug(String debugMsg, String senderID); // Send debug message to Raspberry Pi
void startListenToUDPbroadcast(); // Listen to UDP broadcasts
void stopListenToUDPbroadcast(); // Stop listening to UDP broadcasts
bool getIdFromUDPbroadcast(int udpMsgLength); // Extract IP address from received broadcast

#endif // wifi_lib_h
