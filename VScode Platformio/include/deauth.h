/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/jbohack/nyanSAO
   ________________________________________
*/

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"

void initDeauthMode();
void deauthMode();
void resetDeauthState();
