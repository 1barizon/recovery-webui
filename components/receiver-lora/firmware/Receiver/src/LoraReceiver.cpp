#include "LoraReceiver.h"
#include "Config.h"
#include "Buzzer.h"
#include <LoRa.h>
#include <SPI.h>

/* LORA RECEIVER MODULE - START */

static int rocket_packet_count    = 0;
static int satellite_packet_count = 0;

/**
 * Check if received CSV line belongs to a known device
 * @param line CSV line received via LoRa
 * @return true if TEAM_ID is #100 or #261
 */
static bool isKnownDevice(const String &line)
{
  return line.startsWith(TEAM_ID_ROCKET) || line.startsWith(TEAM_ID_SATELLITE);
}

/**
 * Check if rocket reported parachute deployment
 * Last field of rocket CSV is pqd (parachute deployed): 0 or 1
 * @param line CSV line received
 * @return true if pqd == 1
 */
static bool parachuteDeployed(const String &line)
{
  int lastComma = line.lastIndexOf(',');
  if (lastComma < 0) return false;
  String pqd = line.substring(lastComma + 1);
  pqd.trim();
  return pqd == "1";
}

/**
 * Initialize the LoRa communication module in continuous receive mode
 * SPI pins must be set explicitly — ESP32-C3 has no default SPI pinout
 * @return true if initialization successful, false otherwise
 */
bool setupLoRa()
{
  // ESP32-C3 requer inicialização explícita do barramento SPI
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SS_LORA);

  LoRa.setPins(SS_LORA, RST_LORA, DIO0_LORA);
  if (!LoRa.begin(LORA_FREQ))
  {
    Serial.println("LoRa initialization failed.");
    return false;
  }
  LoRa.setSyncWord(SYNC_WORD);
  LoRa.receive();
  return true;
}

/**
 * Check for incoming LoRa packet, validate TEAM_ID,
 * prefix RSSI/SNR and emit CSV line on Serial USB.
 *
 * Output format: RSSI,SNR,<original CSV line>
 * Example: -87,9.2,#100,12340,42,1523.4,24.1,nan,1013.2,...,1
 */
void handleLoRaPacket()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;

  String received = "";
  while (LoRa.available())
  {
    received += (char)LoRa.read();
  }
  received.trim();

  if (!isKnownDevice(received))
  {
    Serial.println("WARN: unknown device packet ignored: " + received.substring(0, 10));
    LoRa.receive();
    return;
  }

  int   rssi = LoRa.packetRssi();
  float snr  = LoRa.packetSnr();

  if (received.startsWith(TEAM_ID_ROCKET))    rocket_packet_count++;
  if (received.startsWith(TEAM_ID_SATELLITE)) satellite_packet_count++;

  // Emit to WebUI: RSSI,SNR + original CSV line intact
  Serial.println(String(rssi) + "," + String(snr, 1) + "," + received);

  if (received.startsWith(TEAM_ID_ROCKET) && parachuteDeployed(received))
  {
    buzzSignal("Activated");
  }
  else
  {
    buzzSignal("Beep");
  }

  LoRa.receive();
}

/* LORA RECEIVER MODULE - END */
