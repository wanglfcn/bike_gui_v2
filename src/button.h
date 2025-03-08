#include <stdio.h>
#include <Arduino.h>
#include "OneButton.h"
#include "ui/ui.h"
#include "consts.h"

TaskHandle_t updateButtonUIHandler;
TaskHandle_t updateButtonClickHandler;

volatile int turnLeftStatus = 0;
volatile int turnRightStatus = 0;
volatile int lightStatus = 0;
volatile int alarmStatus = 0;

unsigned long lightLastUpdateTs = 0;

OneButton turnLeftBtn;
OneButton turnRightBtn;
OneButton alarmBtn;
OneButton lightBtn;

void triggerTurnLeft()
{
	if (alarmStatus != 0)
	{
		digitalWrite(LEFT_CTL_PIN, LOW);
		digitalWrite(RIGHT_CTL_PIN, LOW);
		alarmStatus = 0;
	}
	if (turnRightStatus != 0)
	{
		digitalWrite(RIGHT_CTL_PIN, LOW);
		turnRightStatus = 0;
	}

	if (turnLeftStatus == 0)
	{
		digitalWrite(LEFT_CTL_PIN, HIGH);
		turnLeftStatus = 1;
		lightLastUpdateTs = millis();
	}
	else
	{
		digitalWrite(LEFT_CTL_PIN, LOW);
		turnLeftStatus = 0;
	}
}

void triggerTurnRight()
{
	if (alarmStatus != 0)
	{
		digitalWrite(LEFT_CTL_PIN, LOW);
		digitalWrite(RIGHT_CTL_PIN, LOW);
		alarmStatus = 0;
	}
	if (turnLeftStatus != 0)
	{
		digitalWrite(LEFT_CTL_PIN, LOW);
		turnLeftStatus = 0;
	}

	if (turnRightStatus == 0)
	{
		digitalWrite(RIGHT_CTL_PIN, HIGH);
		turnRightStatus = 1;
		lightLastUpdateTs = millis();
	}
	else
	{
		digitalWrite(RIGHT_CTL_PIN, LOW);
		turnRightStatus = 0;
	}
}

void triggerAlarm()
{
	if (turnLeftStatus != 0)
	{
		digitalWrite(LEFT_CTL_PIN, LOW);
		turnLeftStatus = 0;
	}

	if (turnRightStatus != 0)
	{
		digitalWrite(RIGHT_CTL_PIN, LOW);
		turnRightStatus = 0;
	}

	if (alarmStatus == 0)
	{
		digitalWrite(LEFT_CTL_PIN, HIGH);
		digitalWrite(RIGHT_CTL_PIN, HIGH);
		alarmStatus = 1;
		lightLastUpdateTs = millis();
	}
	else
	{
		digitalWrite(LEFT_CTL_PIN, LOW);
		digitalWrite(RIGHT_CTL_PIN, LOW);
		alarmStatus = 0;
	}
}

void triggerLight()
{
	if (lightStatus == 0)
	{
		digitalWrite(LIGHT_CTL_PIN, HIGH);
		lightStatus = 1;
	}
	else
	{
		digitalWrite(LIGHT_CTL_PIN, LOW);
		lightStatus = 0;
	}
	Serial.print("trigger light ");
	Serial.println(lightStatus);
}

void updateLightUi()
{
	if (lightStatus == 1)
	{
		lv_obj_clear_flag(ui_lightIcon, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_lightIcon, LV_OBJ_FLAG_HIDDEN);
	}
}

void IRAM_ATTR checkTicks()
{
	// include all buttons here to be checked
	turnLeftBtn.tick();
	turnRightBtn.tick();
	alarmBtn.tick();
	lightBtn.tick();
}

void initBtnUI()
{
	lv_obj_add_flag(ui_lightIcon, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
}

void updateLightInScreen()
{
	int freq = 600;
	if (turnLeftStatus != 0 || turnRightStatus != 0 || alarmStatus != 0)
	{
		long diff = millis() - lightLastUpdateTs;
		int isShow = (diff / freq) % 2;
		if (isShow == 0)
		{
			if (alarmStatus != 0)
			{
				// set show left
				lv_obj_clear_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
				lv_obj_clear_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
			}
			else if (turnLeftStatus != 0)
			{
				// set show left
				lv_obj_clear_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
			}
			else
			{
				// set show right
				lv_obj_clear_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
			}
		}
		else
		{
			// set hide left
			lv_obj_add_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
			// set hide right
			lv_obj_add_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
		}
	}
	else
	{
		// set hide left
		lv_obj_add_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
		// set hide right
		lv_obj_add_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
	}
}


void updateButtonClickTask(void *param);
void updateButtonUITask(void *param);

void initButton()
{
	// setup OneButton
	pinMode(LEFT_BTN_PIN, INPUT_PULLUP);
	pinMode(RIGHT_BTN_PIN, INPUT_PULLUP);
	pinMode(LIGHT_BTN_PIN, INPUT_PULLUP);
	pinMode(ALARM_BTN_PIN, INPUT_PULLUP);

	pinMode(LIGHT_CTL_PIN, OUTPUT);
	digitalWrite(LIGHT_CTL_PIN, LOW);

	turnLeftBtn.setup(LEFT_BTN_PIN, INPUT_PULLUP, true);
	turnLeftBtn.attachClick(triggerTurnLeft);
	attachInterrupt(digitalPinToInterrupt(LEFT_BTN_PIN), checkTicks, CHANGE);

	pinMode(LEFT_CTL_PIN, OUTPUT);
	digitalWrite(LEFT_CTL_PIN, LOW);

	turnRightBtn.setup(RIGHT_BTN_PIN, INPUT_PULLUP, true);
	turnRightBtn.attachClick(triggerTurnRight);
	pinMode(RIGHT_CTL_PIN, OUTPUT);
	digitalWrite(RIGHT_CTL_PIN, LOW);
	attachInterrupt(digitalPinToInterrupt(RIGHT_BTN_PIN), checkTicks, CHANGE);

	alarmBtn.setup(ALARM_BTN_PIN, INPUT_PULLUP, true);
	alarmBtn.attachClick(triggerAlarm);
	attachInterrupt(digitalPinToInterrupt(ALARM_BTN_PIN), checkTicks, CHANGE);

	lightBtn.setup(LIGHT_BTN_PIN, INPUT_PULLUP, true);
	lightBtn.attachClick(triggerLight);
	attachInterrupt(digitalPinToInterrupt(LIGHT_BTN_PIN), checkTicks, CHANGE);

	initBtnUI();

	xTaskCreate(
		updateButtonClickTask,	  /* Function to implement the task */
		"updateButtonClickTask",  /* Name of the task */
		10000,					  /* Stack size in words */
		NULL,					  /* Task input parameter */
		10,						  /* Priority of the task */
		&updateButtonClickHandler /* Task handle. */
	);

	xTaskCreate(
		updateButtonUITask,	   /* Function to implement the task */
		"updateButtonUITask",  /* Name of the task */
		10000,				   /* Stack size in words */
		NULL,				   /* Task input parameter */
		1,					   /* Priority of the task */
		&updateButtonUIHandler /* Task handle. */
	);
}

void updateButtonUITask(void *param)
{
	for (;;)
	{
		updateLightInScreen();
		updateLightUi();

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

void updateButtonClickTask(void *param)
{
	for (;;)
	{
		checkTicks();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}