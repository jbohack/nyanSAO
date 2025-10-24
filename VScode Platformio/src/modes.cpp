/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanSAO
   ________________________________________
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "modes.h"

extern Adafruit_NeoPixel strip;

void bootSequence() {
  for (int cycle = 0; cycle < 20; cycle++) {
    for (int i = 0; i < 10; i++) {
      int brightness = (sin((i + cycle) * 0.5) + 1) * 40;
      strip.setPixelColor(i, strip.Color(brightness, 0, brightness * 0.8));
    }
    strip.setPixelColor(10, 0x404040);
    strip.setPixelColor(11, 0x404040);
    strip.show();
    delay(50);
  }
  strip.clear();
  strip.show();
  delay(100);
}

static inline void setEyes() {
  static unsigned long lastBlink = 0;
  static unsigned long nextBlinkDelay = 3000;
  unsigned long now = millis();
  unsigned long elapsed = now - lastBlink;

  if (elapsed >= nextBlinkDelay) {
    lastBlink = now;
    nextBlinkDelay = random(3000, 6000);
  }

  if (elapsed < 50) {
    strip.setPixelColor(EYE_LEFT, 0x303030);
    strip.setPixelColor(EYE_RIGHT, 0x303030);
  } else if (elapsed < 100) {
    strip.setPixelColor(EYE_LEFT, 0x202020);
    strip.setPixelColor(EYE_RIGHT, 0x202020);
  } else if (elapsed < 150) {
    strip.setPixelColor(EYE_LEFT, 0x101010);
    strip.setPixelColor(EYE_RIGHT, 0x101010);
  } else if (elapsed < 200) {
    strip.setPixelColor(EYE_LEFT, 0);
    strip.setPixelColor(EYE_RIGHT, 0);
  } else if (elapsed < 250) {
    strip.setPixelColor(EYE_LEFT, 0x101010);
    strip.setPixelColor(EYE_RIGHT, 0x101010);
  } else if (elapsed < 300) {
    strip.setPixelColor(EYE_LEFT, 0x202020);
    strip.setPixelColor(EYE_RIGHT, 0x202020);
  } else if (elapsed < 350) {
    strip.setPixelColor(EYE_LEFT, 0x303030);
    strip.setPixelColor(EYE_RIGHT, 0x303030);
  } else {
    strip.setPixelColor(EYE_LEFT, 0x404040);
    strip.setPixelColor(EYE_RIGHT, 0x404040);
  }
}

void nyanClassic() {
  static const uint8_t INTERVAL = 80;
  static const uint32_t COLORS[6] = {
    0x640000, 0x643200, 0x5A5A00, 0x006400, 0x000064, 0x500050
  };
  static int position = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    strip.setPixelColor(i, COLORS[(i + position) % 6]);
  }
  position = (position + 1) % 6;

  setEyes();
  strip.show();
}

void breathingCyan() {
  static const uint8_t INTERVAL = 18;
  static int brightness = B(1);
  static int direction = 1;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  brightness += direction * 2;
  if (brightness >= B(8) || brightness <= B(1)) {
    direction = -direction;
  }

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    strip.setPixelColor(i, strip.Color(0, brightness * 0.8, brightness));
  }
  setEyes();
  strip.show();
}

void rainbow() {
  static const uint8_t INTERVAL = 35;
  static const uint16_t HUE_STEP = 150;
  static uint16_t hueOffset = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    uint16_t pixelHue = hueOffset + (i * 65536L / 10);
    uint32_t color = strip.ColorHSV(pixelHue, 240, B(9));
    strip.setPixelColor(i, strip.gamma32(color));
  }

  setEyes();
  strip.show();
  hueOffset += HUE_STEP;
}

void nyanTrail() {
  static const uint8_t INTERVAL = 80;
  static const uint32_t COLORS[6] = {
    0x640000, 0x643200, 0x5A5A00, 0x006400, 0x000064, 0x500050
  };
  static int position = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  int clearPixel = (position - 6 + 10) % 10;
  strip.setPixelColor(clearPixel, 0);

  for (int i = 0; i < 6; i++) {
    int pixel = (position - i + 10) % 10;
    strip.setPixelColor(pixel, COLORS[i]);
  }

  setEyes();
  strip.show();
  position = (position + 1) % 10;
}

void sparkle() {
  static const uint8_t INTERVAL = 70;
  static const uint8_t SPAWN_CHANCE = 25;
  static unsigned long lastUpdate = 0;
  static int fade[10] = {0};

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    if (fade[i] > 0) {
      fade[i] -= 20;
      if (fade[i] < 0) fade[i] = 0;
    }
    uint8_t blue = fade[i] * 0.9;
    strip.setPixelColor(i, strip.Color(fade[i], fade[i], blue));
  }

  if (random(100) < SPAWN_CHANCE) {
    fade[random(STRIP_START, STRIP_END + 1)] = B(7);
  }

  setEyes();
  strip.show();
}

void dualPulse() {
  static const uint8_t INTERVAL = 12;
  static int brightness = B(1);
  static int direction = 1;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  brightness += direction * 3;
  if (brightness >= B(7) || brightness <= B(1)) {
    direction = -direction;
  }

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    uint32_t color = (i < 5)
      ? strip.Color(brightness, 0, brightness * 0.6)
      : strip.Color(0, brightness * 0.7, brightness);
    strip.setPixelColor(i, color);
  }

  setEyes();
  strip.show();
}

void theaterChase() {
  static const uint8_t INTERVAL = 150;
  static const uint8_t SPACING = 3;
  static int offset = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    uint32_t color = ((i + offset) % SPACING == 0) ? strip.Color(B(6), 0, B(8)) : 0;
    strip.setPixelColor(i, color);
  }

  setEyes();
  strip.show();
  offset = (offset + 1) % SPACING;
}

void fireFlicker() {
  static const uint8_t INTERVAL = 60;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    uint8_t brightness = random(B(4), B(8));
    strip.setPixelColor(i, strip.Color(brightness, brightness * 0.3, 0));
  }

  setEyes();
  strip.show();
}

void matrixRain() {
  static const uint8_t INTERVAL = 100;
  static const uint8_t SPAWN_CHANCE = 40;
  static int drops[10] = {0};
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    if (drops[i] > 0) {
      drops[i] -= 25;
      if (drops[i] < 0) drops[i] = 0;
    }
    strip.setPixelColor(i, strip.Color(0, drops[i], 0));
  }

  if (random(100) < SPAWN_CHANCE) {
    drops[random(STRIP_START, STRIP_END + 1)] = B(7);
  }

  setEyes();
  strip.show();
}

void fedMode() {
  static const uint8_t INTERVAL = 180;
  static bool state = false;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  state = !state;
  for (int i = STRIP_START; i <= STRIP_END; i++) {
    strip.setPixelColor(i, state ? strip.Color(B(6), 0, 0) : strip.Color(0, 0, B(6)));
  }
  setEyes();
  strip.show();
}

void waveform() {
  static const uint8_t INTERVAL = 90;
  static const float FREQUENCY = 0.5;
  static int position = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    uint8_t brightness = (sin((i + position) * FREQUENCY) + 1) * B(4);
    strip.setPixelColor(i, strip.Color(brightness, 0, brightness * 0.8));
  }

  setEyes();
  strip.show();
  position++;
}

void strobe() {
  static const uint8_t INTERVAL = 80;
  static bool state = false;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  state = !state;
  for (int i = STRIP_START; i <= STRIP_END; i++) {
    strip.setPixelColor(i, state ? strip.Color(B(8), B(8), B(8)) : 0);
  }
  setEyes();
  strip.show();
}

void colorWipe() {
  static const uint8_t INTERVAL = 120;
  static const uint32_t COLORS[3] = {0x640000, 0x006400, 0x000064};
  static int position = 0;
  static int colorIndex = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  strip.setPixelColor(position, COLORS[colorIndex]);
  setEyes();
  strip.show();

  position++;
  if (position > STRIP_END) {
    position = STRIP_START;
    colorIndex = (colorIndex + 1) % 3;
    for (int i = STRIP_START; i <= STRIP_END; i++) {
      strip.setPixelColor(i, 0);
    }
  }
}

void glitch() {
  static const uint8_t INTERVAL = 50;
  static const uint8_t GLITCH_CHANCE = 30;
  static const uint32_t COLORS[4] = {0x780000, 0x007800, 0x000078, 0x780078};
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  for (int i = STRIP_START; i <= STRIP_END; i++) {
    strip.setPixelColor(i, (random(100) < GLITCH_CHANCE) ? COLORS[random(4)] : 0);
  }

  setEyes();
  strip.show();
}

void rainbowChase() {
  static const uint8_t INTERVAL = 80;
  static const uint16_t HUE_STEP = 512;
  static int position = 0;
  static uint16_t hue = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < INTERVAL) return;
  lastUpdate = millis();

  strip.setPixelColor(position, 0);

  for (int i = 0; i < 4; i++) {
    int pixel = (position + i) % 10;
    strip.setPixelColor(pixel, strip.ColorHSV(hue + (i * 16384), 255, 120));
  }

  setEyes();
  strip.show();
  position = (position + 1) % 10;
  hue += HUE_STEP;
}