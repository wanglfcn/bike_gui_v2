#include <Arduino.h>
#include <QMC5883LCompass.h>
#include <Adafruit_INA219.h>
#include <Adafruit_AHTX0.h>
#include "ui/ui.h"
#include "consts.h"

QMC5883LCompass compass;
Adafruit_INA219 ina219;
Adafruit_AHTX0 aht;

TaskHandle_t updateSensorHandler;


void updateTemperature()
{
  sensors_event_t humidity, temp;

  aht.getEvent(&humidity, &temp);
  char data[30];
  sprintf(data, "%d°C", (int)(temp.temperature));

  lv_label_set_text(ui_tempLabel, data);
}

void updateCompassDirection()
{

  compass.read();

  int azimuth = compass.getAzimuth();
  int x = azimuth;
  int v = 0;
  if (x >= 100 && x <= 120)
  {
    v = map(x, 100, 120, 105, 267);
  }
  else if (x > 120 && x <= 170)
  {
    v = map(x, 120, 170, 267, 501);
    if (v > 305)
    {
      v = 305 - v;
    }
  }
  else if (x >= 170 && x <= 180 || x < 0 && x < -170)
  {
    if (x < 0)
    {
      x += 360;
    }
    v = map(x, 170, 190, -190, -43);
  }
  else
  {
    x = x + 360;
    v = map(x, 190, 460, -43, -105);
  }

  // update compass offset
  lv_obj_set_x(ui_compass, v);
  char data[30];
  sprintf(data, "%d", azimuth);

  // lv_label_set_text(ui_speedUnit,data);
  //    lv_label_set_text(ui_batteryLevel, data);

  // Serial.print("compass direction: ");
  // Serial.println(azimuth);
}

void updateBatteryLevel()
{
  // Read voltage and current from INA219.
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();

  // Compute load voltage, power, and milliamp-hours.
  float loadvoltage = busvoltage + (shuntvoltage / 1000);

   Serial.print("battery voltage: ");
   Serial.println(loadvoltage);

  // range 7.4-8.4?
  float minV = 3.7;
  float maxV = 4.2;
  int percentage = (int)((100 * (loadvoltage - minV)) / (maxV - minV));
  // update battery
  if (percentage > 100)
  {
    percentage = 100;
  }
  if (percentage < 0)
  {
    percentage = 0;
  }

  char data[20];
  sprintf(data, "%d%%", percentage);
  lv_label_set_text(ui_batteryLevel, data);

  lv_bar_set_value(ui_batteryInd, percentage, LV_ANIM_OFF);
  if (percentage < 25)
  {
    // red
    lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  }
  else if (percentage < 50)
  {
    // yellow
    lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0xFFFF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  }
  else
  {
    lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0x008000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  }
}

void updateSensorTask(void *param);

void initSensor()
{
  bool result = Wire.setPins(IIC_SDA, IIC_SCL);

  result = Wire.begin(IIC_SDA, IIC_SCL, 0);

  Serial.print("begin i2c pins ");
  Serial.println(result);
  compass.init();
  compass.setCalibrationOffsets(-633.00, 677.00, 1440.00);
  compass.setCalibrationScales(1.45, 1.35, 0.64);
  // Try to initialize the INA219
  if (!ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
  }
  if (!aht.begin())
  {
    Serial.println("Could not find AHT30 sensor!");
  }

  xTaskCreate(
      updateSensorTask,    /* Function to implement the task */
      "updateSensorTask",  /* Name of the task */
      10000,               /* Stack size in words */
      NULL,                /* Task input parameter */
      1,                   /* Priority of the task */
      &updateSensorHandler /* Task handle. */
  );
}

void updateSensorTask(void *param)
{
  for (;;)
  {
    updateBatteryLevel();
    updateCompassDirection();
    updateTemperature();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}