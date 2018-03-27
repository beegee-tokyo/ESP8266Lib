#include "ledLib.h"

/** Timer for flashing red detection/alarm/activity LED */
Ticker actFlasher;
/** Timer for flashing blue communication LED */
Ticker comFlasher;
/** Timer for flashing both blue and red leds */
Ticker doubleFlasher;

/** Port for blue LED */
uint8_t comLED = 2;
/** Port for red LED */
uint8_t actLED = 0;

/** Flag if activity LED flashing is active */
bool actLedAttached = false;
/** Flag if communication LED flashing is active */
bool comLedAttached = false;

/**
 * Initialize LED pins
 */
void initLeds(uint8_t reqComLED, uint8_t reqActLED) {
	comLED = reqComLED;
	actLED = reqActLED;
	
	pinMode(comLED, OUTPUT); // Communication LED blue
	pinMode(actLED, OUTPUT); // Communication LED red
	digitalWrite(comLED, HIGH); // Turn off blue LED
	digitalWrite(actLED, HIGH); // Turn off red LED
}

/**
 * Start flashing of red led
 */
void actLedFlashStart(float flashTime) {
	if (actLedAttached) {
		actFlasher.detach();
	}
	actFlasher.attach(flashTime, actLedFlash);
	actLedAttached = true;
}

/**
 * Start flashing of blue led
 */
void comLedFlashStart(float flashTime) {
	if (comLedAttached) {
		comFlasher.detach();
	}
	comFlasher.attach(flashTime, comLedFlash);
	comLedAttached = true;
}

/**
 * Start flashing of both led
 */
void doubleLedFlashStart(float flashTime) {
	if (actLedAttached) {
		actFlasher.detach();
	}
	if (comLedAttached) {
		comFlasher.detach();
	}
	digitalWrite(actLED, LOW); // Turn on red LED
	digitalWrite(comLED, HIGH); // Turn off blue LED
	doubleFlasher.attach(flashTime, doubleLedFlash);
	actLedAttached = true;
	comLedAttached = true;
}

/**
 * Start flashing of red led
 */
void actLedFlashStop() {
	actLedAttached = false;
	digitalWrite(actLED, HIGH); // Turn off red LED
	actFlasher.detach();
}

/**
 * Start flashing of blue led
 */
void comLedFlashStop() {
	comLedAttached = false;
	digitalWrite(comLED, HIGH); // Turn off blue LED
	comFlasher.detach();
}

/**
 * Start flashing of both led
 */
void doubleLedFlashStop() {
	actLedAttached = false;
	comLedAttached = false;
	digitalWrite(actLED, HIGH); // Turn off red LED
	digitalWrite(comLED, HIGH); // Turn off blue LED
	doubleFlasher.detach();
}

/**
 * Change status of red led on each call
 * called by Ticker actFlasher
 */
void actLedFlash() {
	int state = digitalRead(actLED);
	digitalWrite(actLED, !state);
}

/**
 * Change status of blue led on each call
 * called by Ticker comFlasher
 */
void comLedFlash() {
	int state = digitalRead(comLED);
	digitalWrite(comLED, !state);
}

/**
 * Change status of blue led on each call
 * called by Ticker comFlasher
 */
void doubleLedFlash() {
	int state = digitalRead(comLED);
	digitalWrite(comLED, !state);
	digitalWrite(actLED, state);
}
