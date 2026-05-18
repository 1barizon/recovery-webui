#include "GpsModule.h"
#include "Config.h"
#include <TinyGPS++.h>

/* GPS MODULE - START */

TinyGPSPlus GPS;

/**
 * Initialize GPS serial port and wait 3 seconds for first data
 * Mirrors the GPS initialization from the rocket firmware v2
 */
void setupGPS()
{
  Serial1.begin(9600, SERIAL_8N1, RX_GPS, TX_GPS);

  unsigned long start = millis();
  while (millis() - start < 3000)
  {
    while (Serial1.available() > 0)
    {
      GPS.encode(Serial1.read());
    }
  }
}

/**
 * Read available bytes from GPS and feed TinyGPS++ parser
 * Must be called every loop() iteration
 */
void readGPS()
{
  while (Serial1.available() > 0)
  {
    GPS.encode(Serial1.read());
  }
}

/**
 * Collect and emit ground station position on Serial USB
 * Format: GS_GPS,hora,data,lat,lon,alt,sat
 * Mirrors GPSData() format from rocket firmware v2
 */
void emitGPSLine()
{
  String time_data = "nan";
  if (GPS.time.isValid())
  {
    char buf[16];
    sprintf(buf, "%02d:%02d:%02d", GPS.time.hour(), GPS.time.minute(), GPS.time.second());
    time_data = String(buf);
  }

  String date_data = "nan";
  if (GPS.date.isValid())
  {
    char buf[16];
    sprintf(buf, "%04d/%02d/%02d", GPS.date.year(), GPS.date.month(), GPS.date.day());
    date_data = String(buf);
  }

  String lat  = "nan";
  String lon  = "nan";
  String alt  = "nan";
  String sats = "0";

  if (GPS.location.isValid())
  {
    lat  = String(GPS.location.lat(), 8);
    lon  = String(GPS.location.lng(), 8);
    alt  = String(GPS.altitude.meters(), 2);
    sats = String(GPS.satellites.value());
  }

  Serial.println("GS_GPS," + time_data + "," + date_data + "," +
                 lat + "," + lon + "," + alt + "," + sats);
}

/* GPS MODULE - END */
