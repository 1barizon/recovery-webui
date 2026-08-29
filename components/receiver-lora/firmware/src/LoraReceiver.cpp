#include <SPI.h>
#include <LoRa.h>
#include "LoraReceiver.h"
#include "config.h"

static bool _loraReady = false;

bool loraInit() {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

    if (!LoRa.begin(LORA_FREQUENCY)) {
        Serial.println("[LoRa] Falha na inicializacao");
        _loraReady = false;
        return false;
    }

    LoRa.setSpreadingFactor(LORA_SF);
    LoRa.setSignalBandwidth(LORA_BW);
    LoRa.setCodingRate4(LORA_CR);
    LoRa.setSyncWord(LORA_SYNC_WORD);
    LoRa.setTxPower(LORA_TX_POWER);
    LoRa.enableCrc();

    // Coloca em modo de recepcao continua
    LoRa.receive();

    _loraReady = true;
    Serial.print("[LoRa] RX OK (");
    Serial.print(LORA_FREQUENCY / 1E6, 0);
    Serial.print("MHz, SF");
    Serial.print(LORA_SF);
    Serial.print(", BW");
    Serial.print(LORA_BW / 1E3, 0);
    Serial.println("kHz)");
    return true;
}

bool loraSend(const String& payload) {
    if (!_loraReady) return false;

    // Pausa RX para transmitir
    LoRa.idle();
    LoRa.beginPacket();
    LoRa.print(payload);
    bool ok = (LoRa.endPacket() == 1);
    // Volta para modo RX
    LoRa.receive();

    if (ok) Serial.println("[LoRa] TX: " + payload);
    else    Serial.println("[LoRa] Falha no envio");
    return ok;
}

int loraLastRSSI() {
    return LoRa.packetRssi();
}

// Cache do tamanho do pacote detectado em loraAvailable(). LoRa.parsePacket()
// so pode ser chamado UMA vez por pacote — a segunda chamada retorna 0 porque
// as flags de IRQ ja foram limpas. loraReceive() consome esse cache.
static int _pendingSize = 0;

bool loraAvailable() {
    if (!_loraReady) return false;
    _pendingSize = LoRa.parsePacket();
    return _pendingSize > 0;
}

String loraReceive() {
    if (!_loraReady) return "";
    if (_pendingSize <= 0) return "";

    int packetSize = _pendingSize;
    _pendingSize = 0;

    String payload;
    payload.reserve(packetSize);
    while (LoRa.available()) {
        payload += (char)LoRa.read();
    }

    // Volta para modo RX (garantia)
    LoRa.receive();

    return payload;
}
