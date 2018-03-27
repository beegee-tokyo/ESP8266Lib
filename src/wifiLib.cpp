#include "wifiLib.h"
#include "wifiLibPrivate.h"

/** Flag if main app uses the watchdog */
bool wdtEnabled = false;

/** Connection status */
byte connStatus = CON_LOST;

/** Selected network 
		true = use primary network
		false = use secondary network
*/
bool usePrimAP = true;

/** Flag if two networks are found */
// bool canSwitchAP = false;

/** Time connection init started */
unsigned long wifiConnectStart;

/** Event handlers for WiFi */
WiFiEventHandler connectedEventHandler;
WiFiEventHandler disconnectedEventHandler;
WiFiEventHandler gotIPEventHandler;

/** WiFiUDP class for listening to UDP broadcasts */
WiFiUDP udpListener;

/**
	 gotCon
	 called when connection to AP was successfull
*/
void gotCon(const WiFiEventStationModeConnected& evt) {
	if (wdtEnabled) wdt_reset();
	if (connStatus == CON_INIT) {
		Serial.println("Got connection after " + String((millis()-wifiConnectStart)/1000) + "s");
		Serial.println("SSID: " + String(evt.ssid));
		Serial.println("Channel: " + String(evt.channel));
		Serial.println("RSSI: " + String(WiFi.RSSI()));
		connStatus = CON_START;
		// Switch LED status to connected but no IP
		comLedFlashStop();
		digitalWrite(actLED, HIGH);	
		digitalWrite(comLED, LOW);
	}
}

/**
	 lostCon
	 called when WiFi got disconnected
*/
void lostCon(const WiFiEventStationModeDisconnected& evt) {
	if (wdtEnabled) wdt_reset();
	if ((connStatus == CON_GOTIP) || (connStatus == CON_START)) {
		Serial.println("Lost connection");
		connStatus = CON_LOST;
		// Switch on both LED's to show disconnect status
		comLedFlashStop();
		digitalWrite(actLED, LOW);	
		digitalWrite(comLED, LOW);
	}
}

/**
	 gotIP
	 called when IP address was assigned
*/
void gotIP(const WiFiEventStationModeGotIP& evt) {
	if (wdtEnabled) wdt_reset();
	if (connStatus == CON_START) {
		Serial.println("Got IP after " + String((millis()-wifiConnectStart)/1000) + "s");
		connStatus = CON_GOTIP;
		Serial.println(WiFi.localIP());
		Serial.println("RSSI: " + String(WiFi.RSSI()));
		// Switch LED status to blinking comLED
		digitalWrite(actLED, HIGH);	
		digitalWrite(comLED, HIGH);
		comLedFlashStart(0.5);
	}
}

/**
	 connectWiFI
	 starts a connection
*/
void connectWiFi() {
	if (wdtEnabled) wdt_reset();
	// Switch on both LED's
	digitalWrite(actLED, LOW);	
	digitalWrite(comLED, LOW);

	// Connect to Wifi.
	WiFi.disconnect();
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(false);
	Serial.println();
	Serial.print("Start connection to ");
	if (wdtEnabled) wdt_reset();
	if (usePrimAP) {
		Serial.println(ssidPrim);
		WiFi.begin(ssidPrim, pwPrim);
	} else {
		Serial.println(ssidSec);
		WiFi.begin(ssidSec, pwSec);
	}
	connStatus = CON_INIT;

	wifiConnectStart = millis();
}
/**
	 connectInit
	 Initiates connection
*/
void connectInit() {
	// Initialize the callback handlers
	connectedEventHandler = WiFi.onStationModeConnected(&gotCon);
	disconnectedEventHandler = WiFi.onStationModeDisconnected(&lostCon);
	gotIPEventHandler = WiFi.onStationModeGotIP(&gotIP);

	// Scan for available WiFi networks
	scanWiFi();

	// Try to connect to a WiFi network
	connectWiFi();
}

/**
	 scanWiFi
	 Scans for available networks 
	 and decides if a switch between
	 allowed networks makes sense

	 @return <code>bool</code>
					True if at least one allowed network was found
*/
bool scanWiFi() {
	/** RSSI for primary network */
	int8_t rssiPrim;
	/** RSSI for secondary network */
	int8_t rssiSec;
	/** Result of this function */
	bool result = false;
	
	wifiConnectStart = millis();
	doubleLedFlashStart(0.25);
	if (wdtEnabled) wdt_reset();
	int apNum = WiFi.scanNetworks();
	if (apNum == 0) {
		Serial.println("Found no networks?????");
		doubleLedFlashStop();
		return false;
	}
	
	byte foundAP = 0;
	bool foundPrim = false;

	if (wdtEnabled) wdt_reset();
	for (int index=0; index<apNum; index++) {
		String ssid = WiFi.SSID(index);
		Serial.println("Found AP: " + ssid + " RSSI: " + WiFi.RSSI(index));
		if (!strcmp((const char*) &ssid[0], ssidPrim)) {
			foundAP++;
			foundPrim = true;
			rssiPrim = WiFi.RSSI(index);
		}
		if (!strcmp((const char*) &ssid[0], ssidSec)) {
			foundAP++;
			rssiSec = WiFi.RSSI(index);
		}
	}

	switch (foundAP) {
		case 0:
			result = false;
			break;
		case 1:
			if (foundPrim) {
				usePrimAP = true;
			} else {
				usePrimAP = false;
			}
			result = true;
			break;
		default:
			if (rssiPrim > rssiSec) {
				usePrimAP = true; // RSSI of primary network is better
			} else {
				usePrimAP = false; // RSSI of secondary network is better
			}
			result = true;
			break;
	}
	doubleLedFlashStop();
	Serial.println("WiFi scan finished after " + String((millis()-wifiConnectStart)/1000) + "s");
	return result;
}

/**
	 checkWiFiStatus
	 Check current WiFi status and try to reconnect if necessary
*/
void checkWiFiStatus() {
	if (wdtEnabled) wdt_reset();
	// Check if connection initialization was successfull
	if ((connStatus == CON_INIT) || (connStatus == CON_START)) {
		if ((millis() - wifiConnectStart) > 10000) {
			Serial.println("WiFi connection failed for more than 10 seconds");
			Serial.println(WiFi.localIP());
			scanWiFi();
			connectWiFi();
		}
	}
	if (wdtEnabled) wdt_reset();
	// Check if connection was lost
	if (connStatus == CON_LOST) {
		Serial.println("WiFi connection lost");
		scanWiFi();
		if (wdtEnabled) wdt_reset();
		connectWiFi();
	}
}

/**
	 sendDebug
	 send debug message over TCP
*/
void sendDebug(String debugMsg, String senderID) {
	if (connStatus == CON_GOTIP) {
		doubleLedFlashStart(0.5);
		/** WiFiClient class to create TCP communication */
		WiFiClient tcpDebugClient;

		if (!tcpDebugClient.connect(ipDebug, tcpDebugPort)) {
			Serial.println("connection to Debug Android " + String(ipDebug[0]) + "." + String(ipDebug[1]) + "." + String(ipDebug[2]) + "." + String(ipDebug[3]) + " failed");
			tcpDebugClient.stop();
			doubleLedFlashStop();
			return;
		}

		// String sendMsg = OTA_HOST;
		debugMsg = senderID + " " + debugMsg;
		tcpDebugClient.print(debugMsg);

		tcpDebugClient.stop();
		doubleLedFlashStop();
	}
}

/**
void sendRpiDebug(String debugMsg, String senderID) {
	 sendDebug
	 send debug message over TCP to Raspberry Pi
*/
void sendRpiDebug(String debugMsg, String senderID) {
	if (connStatus == CON_GOTIP) {
		doubleLedFlashStart(0.5);
		/** WiFiClient class to create TCP communication */
		WiFiClient tcpDebugClient;

		if (!tcpDebugClient.connect(ipMonitor, tcpRpiDebugPort)) {
			Serial.println("connection to Debug PC " + String(ipMonitor[0]) + "." + String(ipMonitor[1]) + "." + String(ipMonitor[2]) + "." + String(ipMonitor[3]) + " failed");
			tcpDebugClient.stop();
			doubleLedFlashStop();
			// Send debug message as well to tablet
			sendDebug(debugMsg, senderID);
			return;
		}

		// String sendMsg = OTA_HOST;
		debugMsg = senderID + " " + debugMsg;
		tcpDebugClient.print(debugMsg);

		tcpDebugClient.stop();

		doubleLedFlashStop();

		// Send debug message as well to tablet
		sendDebug(debugMsg, "");
	}
}

/**
	startListenToUDPbroadcast
	Start to listen for	UDP broadcast message
*/
void startListenToUDPbroadcast() {
	// Start UDP listener
	udpListener.begin(udpBcPort);
}

/**
	stopListenToUDPbroadcast
	Stop to listen for	UDP broadcast message
*/
void stopListenToUDPbroadcast() {
	// Start UDP listener
	udpListener.stop();
}

/**
	getIdFromUDPbroadcast
	Get active device IDs and IPs UDP broadcast message
*/
bool getIdFromUDPbroadcast(int udpMsgLength) {
	doubleLedFlashStart(0.1);
	bool result = false;
	byte udpPacket[udpMsgLength+1];
	IPAddress udpIP = udpListener.remoteIP();

	udpListener.read(udpPacket, udpMsgLength);
	udpPacket[udpMsgLength] = 0;

	udpListener.flush(); // empty UDP buffer for next packet

	// String debugMsg = "UDP broadcast from ";
	// udpIP = udpListener.remoteIP();
	// debugMsg += "Sender IP: " + String(udpIP[0]) + "." + String(udpIP[1]) + "." + String(udpIP[2]) + "." + String(udpIP[3]);
	// debugMsg += "\n Msg: " + String((char *)udpPacket);
	// sendDebug(debugMsg, "WIFI");

	/** Buffer for incoming JSON string */
	DynamicJsonBuffer jsonInBuffer;
	/** Json object for incoming data */
	JsonObject& jsonIn = jsonInBuffer.parseObject((char *)udpPacket);
	if (jsonIn.success() && jsonIn.containsKey("de")) {
		String device = jsonIn["de"]; //String device = jsonIn["device"];
		// String test = "";
		if (device == lightID) { // Found id for connected light device
			lightIp = udpIP;
			result = true;
			// test = "Light ID: " + lightIp.toString();
		} else if (device == camID) { // Found id for connected camera device
			camIp = udpIP;
			result = true;
			// test = "Camera ID: " + camIp.toString();
		} else if (device == secID) { // Found id for connected security device
			secIp = udpIP;
			result = true;
			// test = "Security ID: " + secIp.toString();
		}
		// if (test != "") {
		// 	Serial.println("Found connected " + test);
		// 	sendDebug(test, "wifi.cpp");
		// }
	}
	doubleLedFlashStop();
	return result;
}
