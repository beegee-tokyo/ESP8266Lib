#include "wifiLib.h"
#include "wifiLibPrivate.h"

/** Connection status */
byte connStatus = CON_LOST;

/** Selected network 
    true = use primary network
		false = use secondary network
*/
bool usePrimAP = true;

/** Flag if two networks are found */
bool canSwitchAP = false;

/** Time connection init started */
unsigned long wifiConnectStart;

/** Event handlers for WiFi */
WiFiEventHandler connectedEventHandler;
WiFiEventHandler disconnectedEventHandler;
WiFiEventHandler gotIPEventHandler;

/**
	 gotCon
	 called when connection to AP was successfull
*/
void gotCon(const WiFiEventStationModeConnected& evt) {
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
	// Switch on both LED's
	digitalWrite(actLED, LOW);	
	digitalWrite(comLED, LOW);

	// Connect to Wifi.
	WiFi.disconnect();
	// delay(500);
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(false);
	Serial.println();
	Serial.print("Start connection to ");
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
	uint8_t rssiPrim;
	/** RSSI for secondary network */
	uint8_t rssiSec;
	/** Result of this function */
	bool result = false;
	
	wifiConnectStart = millis();
	doubleLedFlashStart(0.25);
	int apNum = WiFi.scanNetworks();
	if (apNum == 0) {
		Serial.println("Found no networks?????");
		doubleLedFlashStop();
		return false;
	}
	
	byte foundAP = 0;
	bool foundPrim = false;

	for (int index=0; index<apNum; index++) {
		String ssid = WiFi.SSID(index);
		Serial.println("Found AP: " + ssid + " RSSI: " + WiFi.RSSI(index));
		if (strcmp((const char*) &ssid[0], ssidPrim)) {
			foundAP++;
			foundPrim = true;
			rssiPrim = WiFi.RSSI(index);
		}
		if (strcmp((const char*) &ssid[0], ssidSec)) {
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
			canSwitchAP = false;
			result = true;
			break;
		default:
			if (rssiPrim < rssiSec) {
				usePrimAP = true; // RSSI of primary network is better
			} else {
				usePrimAP = false; // RSSI of secondary network is better
			}
			canSwitchAP = true;
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
	// Check if connection initialization was successfull
	if ((connStatus == CON_INIT) || (connStatus == CON_START)) {
		if ((millis() - wifiConnectStart) > 10000) {
			Serial.println("WiFi connection failed for more than 10 seconds");
			Serial.println(WiFi.localIP());
			// Toggle WiFi AP
			usePrimAP = !usePrimAP;
			connectWiFi();
		}
	}
	// Check if connection was lost
	if (connStatus == CON_LOST) {
		Serial.println("WiFi connection lost");
		// Toggle WiFi AP if we can
		if (canSwitchAP) {
			usePrimAP = !usePrimAP;
		}
		connectWiFi();
	}
}

/**
	 initOTA
	 initializes OTA updates
*/
void initOTA() {
	// Create device ID from MAC address
	char hostApName[] = "MHC-Lan-xxxxxxxx";
	String macAddress = WiFi.macAddress();
	hostApName[8] = macAddress[0];
	hostApName[9] = macAddress[1];
	hostApName[10] = macAddress[9];
	hostApName[11] = macAddress[10];
	hostApName[12] = macAddress[12];
	hostApName[13] = macAddress[13];
	hostApName[14] = macAddress[15];
	hostApName[15] = macAddress[16];

	ArduinoOTA.setHostname(hostApName);
	Serial.println("OTA host name: "+ String(hostApName));

	ArduinoOTA.onStart([]() {
		Serial.println("OTA start");
		digitalWrite(comLED, LOW); // Turn on blue LED
		digitalWrite(actLED, HIGH); // Turn off red LED
	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		digitalWrite(comLED, !digitalRead(comLED)); // Toggle blue LED
		digitalWrite(actLED, !digitalRead(actLED)); // Toggle red LED
	});

	ArduinoOTA.onError([](ota_error_t error) {
		digitalWrite(comLED, LOW); // Turn on blue LED
		digitalWrite(actLED, LOW); // Turn on red LED
	});

	ArduinoOTA.onEnd([]() {
		digitalWrite(comLED, HIGH); // Turn off blue LED
		digitalWrite(actLED, HIGH); // Turn off red LED
	});

	// Start OTA server.
	ArduinoOTA.begin();

	MDNS.addServiceTxt("arduino", "tcp", "board", "ESP8266");
}
