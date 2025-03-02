#include <Arduino.h>
#include <QMC5883LCompass.h>
#include <Adafruit_INA219.h>
#include <Adafruit_AHTX0.h>
#include "ui/ui.h"


QMC5883LCompass compass;
Adafruit_INA219 ina219;
Adafruit_AHTX0 aht;

unsigned long sensorLastUpdateTs = 0;

void scanI2c() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    } else {

      Serial.print("Unknown error at address 0x");
      Serial.print(address,HEX);
      Serial.print("with code");
      Serial.println(error);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}


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
    if (x >= 100 && x <= 120) {
      v = map(x, 100, 120, 105, 267);
    } else if (x > 120 && x <= 170) {
     v = map(x, 120, 170, 267, 501);
     if (v > 305) {
        v = 305 - v;
      }
    } else if (x >= 170 && x <= 180 || x <0 && x < -170) {
        if (x< 0) {
          x += 360;
        }
        v = map(x, 170, 190, -190, -43);
    } else {
      x = x + 360;
    v = map(x, 190, 460, -43, -105);
  }

    // update compass offset
    lv_obj_set_x(ui_compass, v);
    char data[30];
    sprintf(data, "%d", azimuth);

//lv_label_set_text(ui_speedUnit,data);
 //   lv_label_set_text(ui_batteryLevel, data);

    //Serial.print("compass direction: ");
    //Serial.println(azimuth);
}

void updateBatteryLevel()
{
    // Read voltage and current from INA219.
    float shuntvoltage = ina219.getShuntVoltage_mV();
    float busvoltage = ina219.getBusVoltage_V();

    // Compute load voltage, power, and milliamp-hours.
    float loadvoltage = busvoltage + (shuntvoltage / 1000);
    // range 7.4-8.4?
    float minV = 7.4;
    float maxV = 8.4;
    int percentage = (int)((100 * (loadvoltage - minV)) / (maxV - minV));
    // update battery
    if (percentage > 100)  {
        percentage = 100;
    }
    if (percentage < 0)
    {
      percentage = 0;
    }
    // Serial.print("battery voltage: ");
    // Serial.println(loadvoltage);

    char data[20];
    sprintf(data, "%d%%", percentage);
    lv_label_set_text(ui_batteryLevel, data);

    lv_bar_set_value(ui_batteryInd, percentage, LV_ANIM_OFF);
    if (percentage < 25) {
        // red
        lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    } else if (percentage < 50) {
        // yellow
        lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0xFFFF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0x008000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
}

void initSensorInSlowCore()
{
    bool result = Wire.setPins(1, 2);

    result = Wire.begin(1, 2, 0);

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
}

void updateSensorInSlowCore(unsigned long now)
{
    if (now - sensorLastUpdateTs < 200)
    {
        return;
    }

    sensorLastUpdateTs = now;
    updateBatteryLevel();
    updateCompassDirection();
    updateTemperature();
}