#pragma once

/**
 * @file payload.h
 * @brief Formatacao de pacotes para retransmissao via Serial
 *
 * O receiver recebe pacotes LoRa do satellite (19 campos CSV) e
 * retransmite via Serial no formato do protocolo Recovery WebUI
 * (21 campos CSV), preenchendo hora/data com o GPS local.
 *
 * Formato de 20 campos (protocolo):
 * TEAM_ID,millis,count,altp,temp,umi,p,gp,gr,gy,ap,ar,ay,hora,data,alt,lat,lon,sat,rssi
 */

#include <Arduino.h>

/**
 * @brief Monta linha CSV de 21 campos para o protocolo Recovery WebUI.
 *
 * @param team_id    Identificador (ex: "#213")
 * @param millis_ts  Timestamp ms desde boot
 * @param count      Contador sequencial de pacotes
 * @param ax,ay,az   Acelerometro (m/s2)
 * @param gx,gy,gz   Giroscopio (rad/s)
 * @param temp       Temperatura (C)
 * @param press      Pressao (hPa)
 * @param hum        Umidade (%)
 * @param altp       Altitude barometrica (m)
 * @param lat        Latitude GPS (graus)
 * @param lon        Longitude GPS (graus)
 * @param alt        Altitude GPS (m)
 * @param sats       Numero de satelites GPS
 * @param hora       Hora GPS do receiver (HHMMSS)
 * @param data_gps   Data GPS do receiver (DDMMYYYY)
 * @param rssi       RSSI do sinal LoRa (dBm)
 * @return String CSV terminada com \n
 */
static String buildProtocolPacket(
    const String &team_id,
    uint32_t millis_ts,
    uint32_t count,
    float ax, float ay, float az,
    float gx, float gy, float gz,
    float temp, float press, float hum,
    float altp,
    float lat, float lon, float alt,
    uint8_t sats,
    uint32_t hora,
    uint32_t data_gps,
    int rssi
) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer),
        "%s,%lu,%u,"
        "%.2f,%.2f,%.2f,%.2f,"
        "%.2f,%.2f,%.2f,"
        "%.2f,%.2f,%.2f,"
        "%lu,%lu,"
        "%.2f,%.6f,%.6f,%u,"
        "%u,%d",
        team_id.c_str(), millis_ts, count,
        altp, temp, hum, press,
        gx, gy, gz,
        ax, ay, az,
        hora, data_gps,
        alt, lat, lon, sats,
        (unsigned)0, rssi
    );
    return String(buffer);
}
