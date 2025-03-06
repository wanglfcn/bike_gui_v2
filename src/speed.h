#include "ui/ui.h"
#include <stdio.h>
#include <Arduino.h>
#include <Preferences.h>
#include "OneButton.h"
#include <SimpleKalmanFilter.h>
#include "consts.h"

#define SPEED_NODE_PER_CYCLE 5

unsigned long speedLastUpdateTs = 0;
unsigned long mileageLastUpdateTs = 0;

volatile long speedCounter = 0;
long totalCount = 0;
long totalDistance = 0;
#define BUFFER_SIZE 3
int speedBuf[BUFFER_SIZE];
int bufIndex = 0;

TaskHandle_t updateSpeedStatHandler;
TaskHandle_t updateSpeedTickHandler;

Preferences prefs;
SimpleKalmanFilter simpleKalmanFilter(10, 10, 0.1);

// OneButton speedBtn;

void IRAM_ATTR updateSpeedCounter()
{
    // Here you can add the code that will be executed when the interrupt occurs
    speedCounter++;
    Serial.print("add speed counter");
    Serial.println(speedCounter);
}

void updateSpeed(unsigned long now)
{
    int speed = 0;
    int distance = 0;

    int count = speedCounter;
    totalCount += count;
    speedCounter = 0;
    // speed is zero when no update after 10s

    if (count > 0)
    {
        unsigned long timeDiff = now - speedLastUpdateTs;
        speed = 2.2 * (int)((count * 1000 * 1.31) / (SPEED_NODE_PER_CYCLE * timeDiff));
    }

    speedBuf[bufIndex] = speed;

    Serial.print("raw speed");
    Serial.println(speed);
    bufIndex = (bufIndex + 1) % BUFFER_SIZE;

    speed = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        speed = speed + speedBuf[i];
    }

    Serial.print("speed sum");
    Serial.println(speed);
    speed = speed / BUFFER_SIZE;

    if (speed > 20)
    {
        speed = 20;
    }

    char data[30];
    sprintf(data, "%d", speed);
    lv_label_set_text(ui_speedValue, data);
    lv_bar_set_value(ui_speedBar, speed * 5, LV_ANIM_OFF);

    distance = (int)((totalCount * 1.31) / SPEED_NODE_PER_CYCLE);
    int driveTime = (int)(now / 60000);
    char driveTimeText[30];
    sprintf(driveTimeText, "%dmin", driveTime);
    lv_label_set_text(ui_timeCost, driveTimeText);

    char currentDistance[30];

    int shortDis = distance % 1000;
    int longDis = distance / 1000;
    sprintf(currentDistance, "%dm", shortDis);
    lv_label_set_text(ui_currentMile, currentDistance);

    sprintf(currentDistance, "%dkm", longDis);
    lv_label_set_text(ui_kmLabel, currentDistance);

    long newTotalDistance = totalDistance + (int)((totalCount * 1.31) / (SPEED_NODE_PER_CYCLE * 1000));
    if (newTotalDistance < 0)
    {
        newTotalDistance = 0;
    }
    char totalDistance[30];
    sprintf(totalDistance, "%dkm", newTotalDistance);
    lv_label_set_text(ui_rangeLabel, totalDistance);

    speedLastUpdateTs = now;
}

void saveTotalDistance(unsigned long now)
{
    if (now - mileageLastUpdateTs > 60000)
    {
        long newTotalDistance = totalDistance + (int)((totalCount * 1.31) / SPEED_NODE_PER_CYCLE);
        prefs.putLong64("mileageV2", newTotalDistance);
        mileageLastUpdateTs = now;
    }
}


void updateSpeedStatTask(void *param);

void updateSpeedTickTask(void *param);

void initSpeed()
{
    prefs.begin("bike_gui");
    mileageLastUpdateTs = millis();
    totalDistance = prefs.getLong64("mileageV2", 0);
    if (totalDistance < 0)
    {
        totalDistance = 0;
    }
    pinMode(SPEED_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(SPEED_PIN), updateSpeedCounter, FALLING);

    xTaskCreate(
        updateSpeedStatTask,    /* Function to implement the task */
        "updateSpeedStatTask",  /* Name of the task */
        10000,                  /* Stack size in words */
        NULL,                   /* Task input parameter */
        1,                      /* Priority of the task */
        &updateSpeedStatHandler /* Task handle. */
    );

    xTaskCreate(
        updateSpeedTickTask,    /* Function to implement the task */
        "updateSpeedTickTask",  /* Name of the task */
        10000,                  /* Stack size in words */
        NULL,                   /* Task input parameter */
        10,                     /* Priority of the task */
        &updateSpeedTickHandler /* Task handle. */
    );
}

void IRAM_ATTR checkSpeedTicks()
{
    // include all buttons here to be checked
    // speedBtn.tick();
}

void updateSpeedStatTask(void *param)
{
    for (;;)
    {
        unsigned long now = millis();
        saveTotalDistance(now);

        updateSpeed(now);
        speedLastUpdateTs = now;

        vTaskDelay(400 / portTICK_PERIOD_MS);
    }
}

void updateSpeedTickTask(void *param)
{

    for (;;)
    {
        checkSpeedTicks();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
