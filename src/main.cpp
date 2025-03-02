#include <Arduino.h>

#include "ui/ui.h"

#include "lcd.h"
#include "button.h"
#include "speed.h"
#include "sensor.h"

TaskHandle_t fastTask;

void initSlow() {
  initLcd();
  initButtonInSlowCore();
  initInSlowCoreForSpeed();
  initSensorInSlowCore();
}

void loopSlow() {
  unsigned long now = millis();

  updateButtonInSlowCore(now);
  updateInSlowCoreForSpeed(now);
  updateSensorInSlowCore(now);

  refreshLcd(now);
}

void FastTaskCode(void* parameter) {
  initButtonInFastCore();
  initInFastCoreForSpeed();
  for (;;) {
    updateButtonInFastCore();
    updateInFastCoreForSpeed();
  }
}


void setup() {
  Serial.begin(115200);
  initSlow();

  xTaskCreatePinnedToCore(
    FastTaskCode, /* Function to implement the task */
    "FastTask",   /* Name of the task */
    10000,        /* Stack size in words */
    NULL,         /* Task input parameter */
    0,            /* Priority of the task */
    &fastTask,    /* Task handle. */
    0);           /* Core where the task should run */
}

void loop() {
  loopSlow();
}
