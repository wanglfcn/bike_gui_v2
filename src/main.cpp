#include <Arduino.h>

#include "ui/ui.h"

#include "lcd.h"
#include "button.h"
#include "speed.h"
#include "sensor.h"

void setup()
{
  Serial.begin(115200);
  initButton();
  initSpeed();
  initSensor();
  initLcd();
}

void loop()
{
  vTaskDelay(50 / portTICK_PERIOD_MS);
}
