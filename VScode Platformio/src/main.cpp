/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanSAO
   ________________________________________
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "modes.h"

#define LED_PIN 3
#define LED_COUNT 12
#define MODE_SELECT 10

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t currentMode = 0;
bool buttonPressed = false;

#define MODE(fn) {fn, #fn}

struct Mode {
  void (*func)();
  const char* name;
};

const Mode modes[] = {
  MODE(nyanClassic),
  MODE(rainbow),
  MODE(nyanTrail),
  MODE(sparkle),
  MODE(dualPulse),
  MODE(theaterChase),
  MODE(fireFlicker),
  MODE(matrixRain),
  MODE(fedMode),
  MODE(breathingCyan),
  MODE(strobe),
  MODE(colorWipe),
  MODE(glitch),
  MODE(rainbowChase),
  MODE(waveform)
};

const uint8_t MODE_COUNT = sizeof(modes) / sizeof(modes[0]);

void checkButton() {
  static bool lastState = HIGH;
  static unsigned long lastChange = 0;
  bool reading = digitalRead(MODE_SELECT);

  if (reading != lastState) lastChange = millis();
  if ((millis() - lastChange) > 50 && reading == LOW && !buttonPressed) {
    buttonPressed = true;
    currentMode = (currentMode + 1) % MODE_COUNT;
    Serial.print("Mode: ");
    Serial.println(modes[currentMode].name);
  }
  if (reading == HIGH) buttonPressed = false;
  lastState = reading;
}

void setup() {
  // Short delay for entering bootloader on RST
  delay(1000);
  
  Serial.begin(115200);

  pinMode(MODE_SELECT, INPUT_PULLUP);

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();

  bootSequence();
}

void loop() {
  checkButton();
  modes[currentMode].func();
}