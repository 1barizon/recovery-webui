#pragma once
#include <Arduino.h>

/* ================================================================
   GPS MODULE — NEO-N8M
   Backup de localização da ground station via Serial1.
   ================================================================ */

void setupGPS();
void readGPS();
void emitGPSLine();
