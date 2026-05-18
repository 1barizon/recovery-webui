#pragma once

/* ================================================================
   Ground Station — Configuração central
   Pinagem conforme esquemático KiCad (ESP32-C3-DevKitM-1)
   ================================================================ */

// ── LoRa RFM95W-868S2 (SPI) ──────────────────────────────────
//  Pinagem conforme esquemático KiCad (U3 → U1 ESP32-C3)
#define SS_LORA    10   // NSS   → IO10
#define RST_LORA    3   // RESET → IO3
#define DIO0_LORA   4   // DIO0  → IO4

//  SPI explícito — obrigatório no ESP32-C3 (sem pinos SPI default)
#define SPI_SCK     6   // SCK   → IO6
#define SPI_MOSI    7   // MOSI  → IO7
#define SPI_MISO    2   // MISO  → IO2

#define LORA_FREQ  868E6  // Hz — igual ao foguete/satélite
#define SYNC_WORD  0xF3   // Igual ao foguete/satélite

// ── GPS NEO-M8N (Serial1) ────────────────────────────────────
//  TXD do GPS vai no RX do ESP32 e vice-versa
#define RX_GPS     28   // TXD NEO-M8N → IO28 ESP32-C3
#define TX_GPS     29   // RXD NEO-M8N → IO29 ESP32-C3

// ── Buzzer ───────────────────────────────────────────────────
#define BUZZER_PIN  9

// ── Serial USB → Recovery WebUI ──────────────────────────────
#define SERIAL_BAUD 115200

// ── IDs dos dispositivos monitorados ─────────────────────────
#define TEAM_ID_ROCKET    "#100"
#define TEAM_ID_SATELLITE "#261"

// ── Intervalo de emissão do GPS da ground station (ms) ───────
#define GPS_EMIT_INTERVAL 2000
