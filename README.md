# ESP8266-Lib    
Here I collect functions that I am using in all my ESP8266 projects.    
It is work in progress and will be updated over time.
***
# Usage
Include **`ledLib.h`** to get access to the LED flash functions of the library    
Include **`wifiLib.h`** to get access to the WiFi connection functions of the library    
Include **`ntpLib.h`** to get access to the NTP and time functions of the library    
***
# Dependencies
[Ticker](https://github.com/esp8266/Arduino/tree/master/libraries/Ticker)    
[ArduinoOTA](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA)    
[ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)    
[ESP8266mDNS](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS)    
[WiFiUDP](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)    
[Time](https://github.com/PaulStoffregen/Time)    
[ArduinoJSON](https://github.com/bblanchon/ArduinoJson)    
***
# Installation
These libraries can be installed in two ways:    
## Manually    
Download the libraries and manually copy them to correct folders.    
## Automatically
In platformio.ini add or uncomment the following lines:
```
lib_deps =
  https://github.com/beegee-tokyo/ESP8266-Lib.git
```    
***
# Available libraries    
***
## wifiLib    
WiFi connection functionalities    

#### Initialize WiFi connection
**`void connectInit()`**    
Initializes WiFi event callbacks, searches if predefined AP's are available and starts AP connection attempt.    
* **`connStatus`** is the status of the connection.    

4 possible status:
* **`CON_INIT`** connection attempt is initializes.    
* **`CON_START`** connection attempt is ongoing. AP was found but no IP assigned yet   
* **`CON_GOTIP`** connected with valid IP.    
* **`CON_LOST`** Disconnected or no connection available.    

#### Check WiFi connection status    
**`void checkWiFiStatus()`**    
Checks if the connection was established, is ongoing or was lost.    
Tries to connect to alternative AP if connection failed or was disconnected.    
Should be called frequently from loop() or by a [Ticker](https://github.com/esp8266/Arduino/tree/master/libraries/Ticker)    

#### Initialize OTA function    
**`void initOTA()`**    
Initialize the over-the-air update function.    
Should be called once after a connection was established.    
***
## ledLib    
LED flashing functions    

#### Initialize LED's
**`void initLeds()`**    
Initialize the LED ports on the default Adafruit ESP8266 Huzzah GPIO ports for the blue (communication) LED and red (activity) LED
**`void initLeds(uint8_t reqComLED = 2, uint8_t reqActLED = 0)`**    
Initialize the LED ports on the default Adafruit ESP8266 Huzzah GPIO ports for the blue (communication) LED and red (activity) LED
* **`reqComLED`** GPIO port for the communication LED    
* **`reqActLED`** GPIO port for the communication LED    

#### Start activity LED flash    
**`void actLedFlashStart(float flashTime)`**    
Starts the activity LED to flash   
* **`flashTime`** Flash time in s    

#### Start communication LED flash    
**`void comLedFlashStart(float flashTime)`**    
Starts the communication LED to flash   
* **`flashTime`** Flash time in s    

#### Start alternating communication and activity LEDs flash    
**`void doubleLedFlashStart(float flashTime)`**    
Starts the activity and communication LEDs to flash alternating    
* **`flashTime`** Flash time in s    

#### Stop activity LED flash    
**`void actLedFlashStop()`**    
Stops the flashing of the activity LED    

#### Stop communication LED flash    
**`void comLedFlashStop()`**    
Stops the flashing of the communication LED    

### Stop alternating communication and activity LEDs flash    
**`void doubleLedFlashStop()`**    
Stops the alternate flashing of the communication and activity LEDs    
***
## ntpLib    
NTP and time format functions    

#### Try to get time from NTP server
**`bool tryGetTime(boolean debugOn)`**    
Try to get time from NTP server    
Returns false if connection fails
* **`debugOn`** Enable debug output over Serial    

#### Get time and date formatted for display   
**`String digitalClockDisplay()`**    
Formats date and time as hh:mm dd.mm.yy and returns it as a string    
***
## spiffsLib    
Save/read MHC configuration on/from SPIFFS functions    

#### Reformat SPIFFS file system
**`bool formatSPIFFS(String otaHost)`**    
Reformat SPIFFS file system
Returns false if format fails
* **`otaHost`** ID for debug message    

#### Get an entry from the configuration   
**`boolean getConfigEntry(String entryID, char *value)`**    
Reads the configuration of an entry and writes it to the char* array    
Returns false if read fails or entry does not exist
* **`entryID`** The name of the config entry to be read    
* **`value`** Pointer to a char array where the content of the entry is written to    

#### Save an entry in the configuration   
**`boolean saveConfigEntry(String entryID, String value)`**    
Write the value given into the config entry entryID    
Returns false if write fails
* **`entryID`** The name of the config entry to be written    
* **`value`** The value of the config entry as string    
