/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanSAO
   ________________________________________
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "modes.h"
#include "deauth.h"

#define LED_PIN 3
#define LED_COUNT 12
#define MODE_SELECT 10

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t currentMode = 0;
bool buttonPressed = false;
bool inDeauthMode = false;
#define HOLD_DURATION 2500

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
  static unsigned long pressStartTime = 0;
  static bool holdDetected = false;
  bool reading = digitalRead(MODE_SELECT);

  if (reading != lastState) {
    if (reading == LOW) {
      pressStartTime = millis();
      buttonPressed = true;
      holdDetected = false;
    } else {
      if (buttonPressed && !holdDetected && !inDeauthMode) {
        unsigned long pressDuration = millis() - pressStartTime;

        if (pressDuration < HOLD_DURATION) {
          currentMode = (currentMode + 1) % MODE_COUNT;
          Serial.print("Mode: ");
          Serial.println(modes[currentMode].name);
        }
      }

      buttonPressed = false;
      holdDetected = false;
    }
    lastState = reading;
  }

  if (reading == LOW && buttonPressed && !holdDetected) {
    if (millis() - pressStartTime >= HOLD_DURATION) {
      holdDetected = true;

      if (inDeauthMode) {
        inDeauthMode = false;
        Serial.println("Exiting deauth mode");
        resetDeauthState();
      } else {
        inDeauthMode = true;
        Serial.println("Entering deauth mode");
        initDeauthMode();
      }
    }
  }
}

// Function to bypass frame validation (required for raw 802.11 frames)
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  (void)arg;
  (void)arg2;
  (void)arg3;
  return 0;
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

  WiFi.mode(WIFI_AP);
  esp_wifi_set_promiscuous(true);

  bootSequence();
}

void loop() {
  checkButton();

  if (inDeauthMode) {
    deauthMode();
  } else {
    modes[currentMode].func();
  }
}