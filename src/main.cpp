#include <Arduino.h>

#include "ui/ui.h"

#include "lcd.h"
#include "button.h"
#include "speed.h"
#include "sensor.h"
#include "consts.h"

void scanI2c();


void setup3()
{
  /*
  48
  47x
  46
  45
  42x
  41
  40
  39
  */
  
  Serial.println("begin");
	pinMode(48, OUTPUT);
	pinMode(47, OUTPUT);
	pinMode(46, OUTPUT);
	pinMode(45, OUTPUT);
	pinMode(42, OUTPUT);
	pinMode(41, OUTPUT);
	pinMode(40, OUTPUT);
	pinMode(39, OUTPUT);

}

void loop3()
{
  int pin = 39;
  Serial.println("low");
  digitalWrite(pin, LOW);
  delay(1000);
  Serial.println("high");
  digitalWrite(pin, HIGH);
  delay(1000);
}

void setup()
{
  Serial.begin(9600);

  Serial.println("begin");
  initLcd();
  Serial.println("aft init lcd");
  initButton();
  initSpeed();
  initSensor();
  Serial.println("init end");
}

void loop()
{
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void scanI2c()
{
  delay(2000);
  bool result = Wire.setPins(IIC_SDA, IIC_SCL);

  result = Wire.begin(IIC_SDA, IIC_SCL, 0);

  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
    else
    {

      Serial.print("Unknown error at address 0x");
      Serial.print(address, HEX);
      Serial.print("with code");
      Serial.println(error);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000); // wait 5 seconds for next scan
}