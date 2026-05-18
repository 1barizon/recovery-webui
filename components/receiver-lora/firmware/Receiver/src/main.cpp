// Library includes
#include <Wire.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <LoRa.h>

// Ground station modules
#include "Config.h"
#include "Buzzer.h"
#include "LoraReceiver.h"
#include "GpsModule.h"

unsigned long previous_gps_millis = 0;

void setup()
{
  Serial.begin(SERIAL_BAUD);
  Wire.begin();

  setupBuzzer();

  // Aguarda estabilização (espelha o comportamento do foguete)
  for (int i = 0; i < 5; i++)
  {
    Serial.println("Initializing...");
    delay(1000);
  }

  // GPS em Serial1 com espera de 3s (igual ao foguete)
  setupGPS();

  // LoRa em modo recepção contínua
  if (!setupLoRa())
  {
    Serial.println("Module configuration error!");
    buzzSignal("Alert");
    delay(3000);
  }
  else
  {
    Serial.println("All modules initialized successfully!");
    buzzSignal("Success");
  }

  // Cabeçalho CSV para o WebUI identificar as colunas
  Serial.println("RSSI,SNR,TEAM_ID,millis,count,altp,temp,umi,p,gp,gr,gy,ap,ar,ay,hora,data,alt,lon,sat,pqd");
}

void loop()
{
  // Lê GPS continuamente
  readGPS();

  // Processa pacotes LoRa disponíveis
  handleLoRaPacket();

  // Emite posição da ground station a cada GPS_EMIT_INTERVAL ms
  unsigned long now = millis();
  if (now - previous_gps_millis >= GPS_EMIT_INTERVAL)
  {
    emitGPSLine();
    previous_gps_millis = now;
  }
}
