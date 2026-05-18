#pragma once
#include <Arduino.h>

/* ================================================================
   BUZZER MODULE
   Sinais idênticos ao firmware v2 do foguete.
   ================================================================ */

void setupBuzzer();

/**
 * Gera sinais sonoros de status
 * @param signal "Alert"     → erro de inicialização (5 bips)
 *               "Success"   → inicialização OK (3 bips)
 *               "Beep"      → pacote recebido (1 bip curto)
 *               "Activated" → paraquedas deployado (tom especial)
 */
void buzzSignal(String signal);
