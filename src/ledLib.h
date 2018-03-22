#ifndef led_lib_h
#define led_lib_h

#include <Arduino.h>
#include <Ticker.h>

/** Variables */
extern uint8_t comLED;
extern uint8_t actLED;

void initLeds(uint8_t reqComLED = 2, uint8_t reqActLED = 0); // defaults to Adafruit Huzzah breakout ports 
void actLedFlashStart(float flashTime); // Flash time in s
void comLedFlashStart(float flashTime); // Flash time in s
void doubleLedFlashStart(float flashTime); // Flash time in s
void actLedFlashStop();
void comLedFlashStop();
void doubleLedFlashStop();
void actLedFlash();
void comLedFlash();
void doubleLedFlash();

#endif // led_lib_h
