#include "Buzzer.h"
#include "Config.h"

/* BUZZER MODULE - START */

void setupBuzzer()
{
  pinMode(BUZZER_PIN, OUTPUT);
}

/**
 * Generate audio signals using the buzzer for status indication
 * @param signal Signal type: "Alert" (error), "Success" (initialized),
 *               "Activated" (parachute deployed), "Beep" (standard)
 */
void buzzSignal(String signal)
{
  int frequency = 500;
  if (signal == "Alert") // Error signal during initialization
  {
    for (int i = 0; i < 5; i++)
    {
      tone(BUZZER_PIN, frequency, 200);
      delay(200 + 150);
    }
  }
  else if (signal == "Success") // Success signal on initialization
  {
    for (int i = 0; i < 3; i++)
    {
      tone(BUZZER_PIN, frequency, 100);
      delay(100 + 100);
    }
  }
  else if (signal == "Activated") // Parachute deployed — distinctive tone on GS
  {
    tone(BUZZER_PIN, 1200, 500);
    delay(600);
    tone(BUZZER_PIN, 800, 300);
  }
  else if (signal == "Beep") // Standard operation beep
  {
    tone(BUZZER_PIN, frequency, 50);
    delay(100);
  }
  else
  {
    Serial.println("Invalid signal!");
  }
}

/* BUZZER MODULE - END */
