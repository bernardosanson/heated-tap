#include <Arduino.h>
#include <Blinker.h>
#include <EEPROM.h>

#define LED_PIN 16
#define HEATER_PIN 5
#define BUTTON_PIN 17

#define TIME_SHORT 180000
#define TIME_MEDIUM 300000
#define TIME_LONG 420000

#define TIME_SHORT_BYTE 1
#define TIME_MEDIUM_BYTE 2
#define TIME_LONG_BYTE 3

#define TIME_WARNING 15000

#define ADDRESS 0

Blinker blinker(LED_PIN);
bool warningOff = false;
unsigned long timeOut = 0;
unsigned long timePressed = 0;

byte timeSave;

void setup() {
	Serial.begin(115200);
	pinMode(LED_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	pinMode(HEATER_PIN, OUTPUT);

	digitalWrite(HEATER_PIN, LOW);

	blinker.setDelay(750);
	
	timeSave = EEPROM.read(ADDRESS);
	if (timeSave != TIME_LONG_BYTE && timeSave != TIME_MEDIUM_BYTE && timeSave != TIME_SHORT_BYTE) {
		EEPROM.write(ADDRESS, TIME_SHORT_BYTE);
		timeSave = TIME_SHORT_BYTE;
	}

	Serial.print("Time: " );
	Serial.println(timeSave);
}

unsigned long getTimeShutdown() {
	unsigned long time = 0;
	if (timeSave == TIME_LONG_BYTE) {
		time = TIME_LONG;
	} else if (timeSave == TIME_MEDIUM_BYTE) {
		time = TIME_MEDIUM;
	} else if (timeSave == TIME_SHORT_BYTE) {
		time = TIME_SHORT;
	}

	Serial.print("TIME: ");
	Serial.println(time);

	return time;
}

bool isHeaterOn() {
	return digitalRead(HEATER_PIN) == HIGH;
}

void turnOff() {
	Serial.println("HEATER: OFF");
	
	digitalWrite(LED_PIN, LOW);
	digitalWrite(HEATER_PIN, LOW);
	timeOut = 0;
	warningOff = false;
	blinker.stop();
}

void turnOn() {
	Serial.println("HEATER: ON");
	
	timeOut = millis() + getTimeShutdown();
	digitalWrite(LED_PIN, HIGH);
	digitalWrite(HEATER_PIN, HIGH);
}

void handleTime() {
	if (warningOff == false && isHeaterOn() == true && timeOut - millis() < TIME_WARNING) {
		warningOff = true;
		blinker.start();
	}

	if (timeOut != 0 && timeOut < millis()) {
		turnOff();
	}
}

void blinkLed(int blinksNumber) {
	for (int x=0; x<blinksNumber; x++) {
		digitalWrite(LED_PIN, LOW);
		delay(800);
		digitalWrite(LED_PIN, HIGH);
		delay(800);
	}
	digitalWrite(LED_PIN, LOW);
}

void changeTime() {
	turnOff();

	if (timeSave == TIME_SHORT_BYTE) {
		timeSave = TIME_MEDIUM_BYTE;
		blinkLed(2);
	} else if (timeSave == TIME_MEDIUM_BYTE) {
		timeSave = TIME_LONG_BYTE;
		blinkLed(3);
	} else if (timeSave == TIME_LONG_BYTE) {
		timeSave = TIME_SHORT_BYTE;
		blinkLed(1);
	}

	EEPROM.write(ADDRESS, timeSave);
	Serial.print("Time: " );
	Serial.println(timeSave);
	timePressed = 0;

	while(digitalRead(BUTTON_PIN) == LOW){
	}
	
}

// a função loop é executada indefinidamentre.
void loop() {
	blinker.blink();

	if (digitalRead(BUTTON_PIN) == LOW) {
		if (timePressed == 0) {
			timePressed = millis() + 5000;
			if (isHeaterOn() == false) {
				turnOn();
			} else {
				if (warningOff == true) {
					blinker.stop();
					warningOff = false;
					turnOn();
				} else {
					turnOff();
				}
			}
		} else if (timePressed < millis()) {
			changeTime();
		}

		delay(1000);
	} else if (timePressed > 0) {
		timePressed = 0;
	}

	handleTime();
}
