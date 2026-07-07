# Protocolo de Comunicação LoRa

Este documento detalha o protocolo de comunicação entre os dispositivos transmissores (foguete/satélite) e o receptor LoRa.

## Índice

- [Visão Geral](#visão-geral)
- [Especificações Técnicas](#especificações-técnicas)
- [Formato dos Pacotes](#formato-dos-pacotes)
- [Campos de Dados](#campos-de-dados)
- [Identificadores de Dispositivos](#identificadores-de-dispositivos)
- [Exemplos de Pacotes](#exemplos-de-pacotes)
- [Tratamento de Erros](#tratamento-de-erros)
- [Boas Práticas](#boas-práticas)

## Visão Geral

O sistema utiliza comunicação LoRa (Long Range) para transmitir dados de telemetria dos dispositivos para a estação base. Os dados são formatados em CSV e transmitidos via UART/Serial.

```
┌─────────────────┐          ┌──────────────┐          ┌─────────────────┐
│   Foguete/Sat   │          │   Módulo     │          │   Computador    │
│   (Transmissor) │  LoRa    │   Receptor   │   USB    │   (Recovery     │
│                 │ ────────>│   LoRa       │ ────────>│    WebUI)       │
└─────────────────┘   433MHz └──────────────┘  Serial  └─────────────────┘
```

## Especificações Técnicas

### Parâmetros LoRa

| Parâmetro        | Valor   | Descrição                                                |
| ---------------- | ------- | -------------------------------------------------------- |
| Frequência       | 433 MHz | Frequência ISM (varia por região)                        |
| Bandwidth        | 125 kHz | Largura de banda                                         |
| Spreading Factor | 7-12    | Fator de espalhamento (configura alcance vs. velocidade) |
| Coding Rate      | 4/5     | Taxa de correção de erros                                |
| TX Power         | 20 dBm  | Potência de transmissão                                  |
| Alcance típico   | 2-10 km | Depende do ambiente e antenas                            |

### Comunicação Serial

| Parâmetro    | Valor      |
| ------------ | ---------- |
| Baudrate     | 115200 bps |
| Data bits    | 8          |
| Parity       | None       |
| Stop bits    | 1          |
| Flow control | None       |
| Encoding     | UTF-8      |

## Formato dos Pacotes

### Estrutura Geral

Cada pacote e uma linha de texto em formato CSV terminada com `\n`.

**Formato completo (21 campos) — receptor para backend:**

```
TEAM_ID,millis,count,altp,temp,umi,p,gp,gr,gy,ap,ar,ay,hora,data,alt,lat,lon,sat,rssi\n
```

**Formato do satellite (19 campos) — transmissor LoRa:**

```
TEAM_ID,millis,count,altp,temp,umi,p,gp,gr,gy,ap,ar,ay,alt,lat,lon,sat,rssi\n
```

Os campos `hora` e `data` nao sao transmitidos pelo satellite (economia de bytes).
O receptor LoRa preenche esses campos com dados do seu GPS local ao retransmitir
via Serial no formato completo de 21 campos.

### Caracteristicas

- **Delimitador**: Virgula (`,`)
- **Terminador**: Line Feed (`\n`)
- **Total de campos**: 21 (completo) / 19 (satellite)
- **Tamanho tipico**: ~130 bytes (satellite) / ~150 bytes (completo)
- **Frequencia**: 1-5 Hz

## Campos de Dados

### Tabela Completa

| #   | Campo   | Tipo   | Unidade  | Faixa          | Descrição                               | Exemplo  |
| --- | ------- | ------ | -------- | -------------- | --------------------------------------- | -------- |
| 1   | TEAM_ID | string | -        | -              | Identificador do dispositivo            | #100     |
| 2   | millis  | uint32 | ms       | 0 - 4294967295 | Tempo desde boot do microcontrolador    | 12345    |
| 3   | count   | uint16 | -        | 0 - 65535      | Contador sequencial de pacotes          | 42       |
| 4   | altp    | float  | m        | -500 - 50000   | Altitude barométrica                    | 150.5    |
| 5   | temp    | float  | °C       | -40 - 85       | Temperatura ambiente                    | 25.3     |
| 6   | umi     | float  | %        | 0 - 100        | Umidade relativa do ar                  | 45.2     |
| 7   | p       | float  | hPa      | 300 - 1100     | Pressão atmosférica                     | 1013.25  |
| 8   | gp      | float  | °/s      | -2000 - 2000   | Giroscópio pitch (eixo Y)               | 0.5      |
| 9   | gr      | float  | °/s      | -2000 - 2000   | Giroscópio roll (eixo X)                | 1.2      |
| 10  | gy      | float  | °/s      | -2000 - 2000   | Giroscópio yaw (eixo Z)                 | -0.3     |
| 11  | ap      | float  | m/s²     | -156 - 156     | Acelerômetro X                          | 0.1      |
| 12  | ar      | float  | m/s²     | -156 - 156     | Acelerômetro Y                          | 0.2      |
| 13  | ay      | float  | m/s²     | -156 - 156     | Acelerômetro Z                          | 9.8      |
| 14  | hora    | uint32 | HHMMSS   | 0 - 235959     | Hora do GPS (preenchida pelo receptor)  | 143045   |
| 15  | data    | uint32 | DDMMYYYY | -              | Data do GPS (preenchida pelo receptor)   | 20012025 |
| 16  | alt     | float  | m        | -500 - 50000   | Altitude GPS                            | 150.0    |
| 17  | lat     | float  | °        | -90 - 90       | Latitude (graus decimais)               | -23.5505 |
| 18  | lon     | float  | °        | -180 - 180     | Longitude (graus decimais)              | -46.6333 |
| 19  | sat     | uint8  | -        | 0 - 255        | Número de satélites GPS                 | 8        |

| 21  | rssi    | int8   | dBm      | -120 - 0       | Intensidade sinal LoRa no receptor      | -75      |

### Detalhamento dos Campos

#### TEAM_ID

```
Formato: #XXX
Valores validos:
  #100 - Foguete principal
  #213 - Satellite (Helike PocketQube)
```

#### millis

```
Tempo em milissegundos desde que o microcontrolador iniciou
Overflow: ~49 dias
Uso: Sincronização temporal, cálculo de intervalo entre pacotes
```

#### count

```
Contador incremental de pacotes transmitidos
Uso: Detectar perda de pacotes, ordenação
```

#### altp (Altitude Barométrica)

```
Calculada a partir da pressão atmosférica
Fórmula típica: h = 44330 * (1 - (P/P0)^0.1903)
Onde: P0 = 1013.25 hPa (pressão ao nível do mar)
Precisão: ±1-2 metros
```

#### Dados IMU (gp, gr, gy, ap, ar, ay)

```
IMU típica: MPU6050, BMI088, ICM-20948
Taxa de amostragem: 100-1000 Hz
Dados enviados: Última leitura antes da transmissão
```

#### GPS (hora, data, alt, lat, lon, sat)

```
Módulos típicos: NEO-6M, NEO-7M, NEO-M8N
Formato hora: HHMMSS (UTC)
Formato data: DDMMYYYY
Coordenadas: Graus decimais (WGS84)
```

#### rssi

```
Medido no receptor LoRa
Valores típicos:
  -30 a -50 dBm: Excelente
  -50 a -70 dBm: Bom
  -70 a -90 dBm: Razoável
  -90 a -110 dBm: Fraco
  < -110 dBm: Muito fraco/perda iminente
```

## Identificadores de Dispositivos

### TEAM_ID #100 - Foguete Principal

**Características:**

- Computador de bordo principal
- Detecção de apogeu
- Acionamento de paraquedas
- Sensores: GPS, IMU, Barômetro

**Dados específicos:**


- Alta taxa de amostragem durante voo ativo

### TEAM_ID #213 - Satellite (Helike PocketQube)

**Caracteristicas:**

- Microsatelite / PocketQube
- Sensores ambientais (temp, umi, pressao, IMU, GPS)
- Rastreamento independente
- Sem atuadores de recuperacao

**Dados especificos:**

- Dados de IMU (acelerometro + giroscopio) relevantes
- Dados ambientais (temperatura, umidade, pressao)


## Exemplos de Pacotes

### Foguete em Solo (Antes do Lançamento)

```
#100,5000,1,150.5,25.3,45.2,1013.25,0.1,0.2,-0.1,0.0,0.1,9.8,143000,20012025,150.0,-23.5505,-46.6333,8,0,-45
```

**Análise:**

- Tempo: 5 segundos desde boot
- Pacote #1
- Altitude: 150.5m (nível do solo local)
- Aceleração Z: 9.8 m/s² (gravidade - foguete parado verticalmente)
- Satélites: 8 (GPS fixo)
- Paraquedas: Fechado
- RSSI: -45 dBm (excelente)

### Foguete em Ascensão

```
#100,15000,30,450.2,20.1,48.5,950.30,150.5,45.2,30.1,5.2,15.3,80.5,143015,20012025,445.0,-23.5505,-46.6333,7,0,-65
```

**Análise:**

- Tempo: 15 segundos desde boot (10s de voo)
- Pacote #30
- Altitude: 450m (300m acima do solo)
- Temperatura: 20.1°C (queda devido à altitude)
- Giroscópio: Valores altos (rotação durante voo)
- Aceleração: Alta (propulsão ativa ou arrasto)
- Paraquedas: Fechado
- RSSI: -65 dBm (bom)

### Foguete em Apogeu

```
#100,25000,50,850.0,15.5,50.2,900.15,5.2,2.1,1.5,0.1,-0.2,9.8,143025,20012025,845.0,-23.5510,-46.6330,6,1,-80
```

**Análise:**

- Tempo: 25 segundos
- Altitude máxima: 850m
- Aceleração: ~9.8 m/s² (queda livre)

- RSSI: -80 dBm (razoável)

### Foguete em Descida

```
#100,45000,90,200.0,24.8,46.0,1010.50,2.1,1.5,0.8,0.2,0.3,12.5,143045,20012025,195.0,-23.5520,-46.6325,8,1,-55
```

**Análise:**

- Tempo: 45 segundos
- Altitude: 200m (descendo)
- Aceleração Z: 12.5 m/s² (desaceleração pelo paraquedas)
- Temperatura: 24.8°C (aquecendo ao descer)
- RSSI: -55 dBm (excelente - mais próximo)

## Tratamento de Erros

### Pacotes Incompletos

**Problema:** Pacote recebido com menos de 21 campos

**Causa:**

- Interferência no sinal
- Pacote cortado
- Erro de transmissão

**Ação do servidor:**

```python
try:
    TEAM_ID,millis,count,altp,temp,umi,p,gp,gr,gy,ap,ar,ay,hora,data,alt,lat,lon,sat,rssi = fields
except ValueError as e:
    print(f"Erro parse: campos insuficientes - {e}")
    # Descarta pacote
    continue
```

### Dados Inválidos

**Validação recomendada (firmware):**

```cpp
// Validar coordenadas GPS
if (lat < -90 || lat > 90 || lon < -180 || lon > 180) {
    // Coordenadas inválidas, usar último valor válido
}

// Validar altitude
if (alt < -500 || alt > 50000) {
    // Altitude suspeita
}

// Validar temperatura
if (temp < -40 || temp > 85) {
    // Sensor com problema
}
```

### Perda de Pacotes

**Detecção:**

```python
last_count = 0
lost_packets = 0

if count != last_count + 1:
    lost = count - last_count - 1
    lost_packets += lost
    print(f"⚠️  {lost} pacotes perdidos!")

last_count = count
```

## Boas Práticas

### Firmware (Transmissor)

1. **Buffering**: Mantenha buffer de pacotes não transmitidos
2. **Retry**: Implemente retransmissão de pacotes críticos
3. **Checksum**: Considere adicionar campo de verificação
4. **Compressão**: Para longas distâncias, comprima dados
5. **Priorização**: Envie dados críticos (GPS, altitude) com maior frequência

### Receptor

1. **Timeout**: Configure timeout adequado (0.5-1s)
2. **Validação**: Valide formato antes de processar
3. **Logging**: Registre TODOS os pacotes (incluindo inválidos)
4. **Timestamp**: Adicione timestamp do servidor
5. **Backup**: Faça backup periódico dos logs

### Exemplo de Código Transmissor (Arduino)

```cpp
void sendTelemetry() {
    char buffer[200];

    snprintf(buffer, sizeof(buffer),
        "#100,%lu,%u,%.1f,%.1f,%.1f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%lu,%lu,%.1f,%.6f,%.6f,%u,%u,%d\n",
        millis(),           // millis
        packetCount++,      // count
        altitude,           // altp
        temperature,        // temp
        humidity,           // umi
        pressure,           // p
        gyro.x, gyro.y, gyro.z,  // gp, gr, gy
        accel.x, accel.y, accel.z, // ap, ar, ay
        gpsTime,            // hora
        gpsDate,            // data
        gpsAltitude,        // alt
        latitude,           // lat
        longitude,          // lon
        satellites,         // sat

        lora.getRSSI()      // rssi
    );

    LoRa.print(buffer);
}
```

## Troubleshooting

### Sinal Fraco (RSSI baixo)

**Soluções:**

- Melhore as antenas (use antenas de ganho)
- Aumente TX power (máximo 20 dBm)
- Aumente Spreading Factor (SF8-SF12)
- Reduza bandwidth
- Verifique obstáculos entre transmissor e receptor

### Alta Taxa de Perda de Pacotes

**Soluções:**

- Reduza taxa de transmissão
- Aumente Coding Rate (4/6, 4/7)
- Verifique interferência em 433 MHz
- Use canal diferente se disponível

### Coordenadas GPS Incorretas

**Causas:**

- GPS sem fix (sat < 4)
- Dados não atualizados
- Módulo GPS com problema

**Solução:**

```cpp
// Apenas envie se GPS válido
if (gps.satellites.value() >= 4 && gps.location.isValid()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
}
```

---

## Referências

- [LoRa Alliance](https://lora-alliance.org/)
- [Semtech LoRa Documentation](https://www.semtech.com/lora)
- [Arduino LoRa Library](https://github.com/sandeepmistry/arduino-LoRa)

---

[← Voltar à Documentação](README.md)
