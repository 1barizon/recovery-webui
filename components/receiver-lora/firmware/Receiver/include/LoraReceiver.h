#pragma once
#include <Arduino.h>

/* ================================================================
   LORA RECEIVER MODULE
   Recepção contínua de pacotes dos dispositivos #100 e #261.
   ================================================================ */

bool setupLoRa();
void handleLoRaPacket();
