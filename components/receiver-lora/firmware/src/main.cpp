#include <Arduino.h>
#include "config.h"
#include "GpsModule.h"
#include "LoraReceiver.h"
#include "payload.h"

static bool loraReady = false;

// ── Contagem de pacotes perdidos e estatisticas ──
static uint32_t lastCount       = 0;    // ultimo count recebido do satellite
static uint32_t totalReceived   = 0;    // total de pacotes recebidos com sucesso
static uint32_t totalLost       = 0;    // total de pacotes perdidos (saltos no count)
static uint32_t totalErrors     = 0;    // total de pacotes com parse falho
static bool     firstPacket     = true; // true ate receber o primeiro pacote

/**
 * @brief Faz parse do CSV de 19 campos recebido do satellite.
 *
 // Formato do satellite (18 campos):
 // TEAM_ID,millis,count,altp,temp,umi,p,gp,gr,gy,ap,ar,ay,alt,lat,lon,sat,rssi
 *
 * Retorna true se o parse foi bem sucedido.
 */
static bool parseSatellitePacket(
    const String &raw,
    String &team_id,
    uint32_t &millis_ts,
    uint32_t &count,
    float &ax, float &ay, float &az,
    float &gx, float &gy, float &gz,
    float &temp, float &press, float &hum,
    float &altp,
    float &lat, float &lon, float &alt,
    uint8_t &sats,
    int &rssi
) {
    // Valida numero de campos
    int commaCount = 0;
    for (unsigned i = 0; i < raw.length(); i++) {
        if (raw[i] == ',') commaCount++;
    }
    if (commaCount < 18) {
        Serial.println("[PARSE] Pacote incompleto: " + String(commaCount + 1) + " campos");
        return false;
    }

    int start = 0;

    auto nextField = [&](String &out) {
        int next = raw.indexOf(',', start);
        if (next == -1) {
            out = raw.substring(start);
            start = raw.length();
        } else {
            out = raw.substring(start, next);
            start = next + 1;
        }
    };

    String field;

    nextField(team_id);          // 0  TEAM_ID
    nextField(field); millis_ts = field.toInt();  // 1  millis
    nextField(field); count = field.toInt();      // 2  count
    nextField(field); altp = field.toFloat();     // 3  altp
    nextField(field); temp = field.toFloat();     // 4  temp
    nextField(field); hum = field.toFloat();      // 5  umi
    nextField(field); press = field.toFloat();    // 6  p
    nextField(field); gx = field.toFloat();       // 7  gp
    nextField(field); gy = field.toFloat();       // 8  gr
    nextField(field); gz = field.toFloat();       // 9  gy
    nextField(field); ax = field.toFloat();       // 10 ap
    nextField(field); ay = field.toFloat();       // 11 ar
    nextField(field); az = field.toFloat();       // 12 ay
    nextField(field); alt = field.toFloat();      // 13 alt
    nextField(field); lat = field.toFloat();      // 14 lat
    nextField(field); lon = field.toFloat();      // 15 lon
    nextField(field); sats = (uint8_t)field.toInt(); // 16 sat
    nextField(field); rssi = field.toInt();       // 17 rssi (placeholder -1)

    return true;
}

/**
 * @brief Atualiza contadores de pacotes perdidos e loga na Serial.
 *
 * Compara o count recebido com o anterior para detectar saltos.
 * No primeiro pacote, apenas inicializa o contador.
 */
static void trackPacketCount(uint32_t currentCount) {
    totalReceived++;

    if (firstPacket) {
        lastCount = currentCount;
        firstPacket = false;
        Serial.println("[STATS] Primeiro pacote recebido — count=" + String(currentCount));
        return;
    }

    // Detecta salto no contador
    if (currentCount == lastCount + 1) {
        // Sem perda — silencioso para nao poluir a Serial
    } else if (currentCount > lastCount + 1) {
        uint32_t lost = currentCount - lastCount - 1;
        totalLost += lost;
        Serial.println("[LOST] " + String(lost) + " pacote(s) perdido(s) — count "
                       + String(lastCount) + " -> " + String(currentCount)
                       + " | total perdidos: " + String(totalLost));
    } else {
        // count voltou (overflow ou reboot do satellite)
        Serial.println("[STATS] Count resetado — " + String(lastCount)
                       + " -> " + String(currentCount)
                       + " (possivel reboot do satellite)");
    }

    lastCount = currentCount;
}

/**
 * @brief Loga estatisticas acumuladas na Serial.
 */
static void logStats() {
    Serial.println("[STATS] Recebidos: " + String(totalReceived)
                   + " | Perdidos: " + String(totalLost)
                   + " | Erros parse: " + String(totalErrors)
                   + " | Taxa perda: "
                   + String(totalReceived + totalLost > 0
                            ? (totalLost * 100) / (totalReceived + totalLost)
                            : 0)
                   + "%");
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(400);
    Serial.println();
    Serial.println("=== Recovery System — LoRa Receiver ===");
    Serial.println();

    gpsInit();

    loraReady = loraInit();

    Serial.println();
    Serial.println("[SYS] Aguardando pacotes LoRa do satellite...");
    Serial.println();
}

void loop() {
    gpsProcess();

    if (!loraReady || !loraAvailable()) {
        delay(10);
        return;
    }

    String raw = loraReceive();
    if (raw.length() == 0) return;

    int rxRssi = loraLastRSSI();
    Serial.print("[LoRa] RX (RSSI=" + String(rxRssi) + "): ");
    Serial.println(raw);

    // Parse do CSV do satellite
    String team_id;
    uint32_t millis_ts, count;
    float ax, ay, az, gx, gy, gz;
    float temp, press, hum, altp;
    float lat, lon, alt;
    uint8_t sats;
    int rssi_placeholder;

    if (!parseSatellitePacket(raw, team_id, millis_ts, count,
                              ax, ay, az, gx, gy, gz,
                              temp, press, hum, altp,
                              lat, lon, alt, sats, rssi_placeholder)) {
        totalErrors++;
        Serial.println("[STATS] Erro de parse #" + String(totalErrors));
        logStats();
        return;
    }

    // Rastreia pacotes perdidos
    trackPacketCount(count);

    // Hora/data do GPS local do receiver
    GpsTimeData gpsTime = gpsGetTimeData();

    // Monta pacote no formato do protocolo (21 campos)
    String protocolPacket = buildProtocolPacket(
        team_id, millis_ts, count,
        ax, ay, az, gx, gy, gz,
        temp, press, hum, altp,
        lat, lon, alt, sats,
        gpsTime.hhmmss,
        gpsTime.ddmmyyyy,
        rxRssi       // RSSI real medido pelo receiver
    );

    // Retransmite via Serial para o Recovery WebUI
    Serial.println(protocolPacket);
}
