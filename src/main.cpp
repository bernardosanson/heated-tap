#include <Arduino.h>
#include <Blinker.h>

#define LED_PIN 8
#define HEATER_PIN 6
#define BOTAO_PIN 4

#define TIME_SHORT 10000
#define TIME_MEDIUM 20000
#define TIME_LONG 30000

#define TIME_WARNING 10000

Blinker blinker(LED_PIN);
bool heaterOn = false;
bool warningOff = false;
unsigned long timeOut = 0;

void setup() {
	pinMode(LED_PIN, OUTPUT);
	pinMode(BOTAO_PIN, INPUT_PULLUP);
	pinMode(HEATER_PIN, OUTPUT);

	digitalWrite(HEATER_PIN, LOW);

	blinker.setDelay(750);
}

void turnOff() {
	digitalWrite(LED_PIN, LOW);
	heaterOn = false;
	timeOut = 0;
	warningOff = false;
	blinker.stop();
}

void turnOn() {
	timeOut = millis() + TIME_MEDIUM;
	digitalWrite(LED_PIN, HIGH);
	heaterOn = true;
}

void handleTime() {
	if (warningOff == false && heaterOn == true && timeOut - millis() < TIME_WARNING) {
		warningOff = true;
		blinker.start();
	}

	if (timeOut != 0 && timeOut < millis()) {
		turnOff();
	}
}

// a função loop é executada indefinidamentre.
void loop() {
	blinker.blink();

	if (digitalRead(BOTAO_PIN) == LOW) {
		if (heaterOn == false) {
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
		delay(1000);
	}

	handleTime();
}


