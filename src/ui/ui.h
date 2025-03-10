// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.2
// LVGL version: 8.3.11
// Project name: moto

#ifndef _MOTO_UI_H
#define _MOTO_UI_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_speedBar;
extern lv_obj_t *ui_compass;
extern lv_obj_t *ui_background;
extern lv_obj_t *ui_Image4;
extern lv_obj_t *ui_Image1;
extern lv_obj_t *ui_batteryLevel;
extern lv_obj_t *ui_Image3;
extern lv_obj_t *ui_lightIcon;
extern lv_obj_t *ui_speedUnit;
extern lv_obj_t *ui_speedValue;
extern lv_obj_t *ui_rangeLabel;
extern lv_obj_t *ui_Image8;
extern lv_obj_t *ui_tempLabel;
extern lv_obj_t *ui_currentMile;
extern lv_obj_t *ui_Image9;
extern lv_obj_t *ui_Image10;
extern lv_obj_t *ui_timeCost;
extern lv_obj_t *ui_batteryInd;
extern lv_obj_t *ui_kmLabel;
extern lv_obj_t *ui_Container1;
extern lv_obj_t *ui_Container2;
extern lv_obj_t *ui_leftArr;
extern lv_obj_t *ui_rightArr;
extern lv_obj_t *ui____initial_actions0;

LV_IMG_DECLARE( ui_img_guage_bg_png);   // assets/guage bg.png
LV_IMG_DECLARE( ui_img_guage_indicator_png);   // assets/guage indicator.png
LV_IMG_DECLARE( ui_img_compas_png);   // assets/compas.png
LV_IMG_DECLARE( ui_img_devider_png);   // assets/devider.png
LV_IMG_DECLARE( ui_img_guage_bg_1_png);   // assets/guage bg_1.png
LV_IMG_DECLARE( ui_img_battery_bg_png);   // assets/battery bg.png
LV_IMG_DECLARE( ui_img_temperature_png);   // assets/temperature.png
LV_IMG_DECLARE( ui_img_light23_png);   // assets/light23.png
LV_IMG_DECLARE( ui_img_range_bg_png);   // assets/range bg.png
LV_IMG_DECLARE( ui_img_range_knob_png);   // assets/range knob.png
LV_IMG_DECLARE( ui_img_clk2_png);   // assets/clk2.png
LV_IMG_DECLARE( ui_img_left_arr_png);   // assets/left_arr.png


LV_FONT_DECLARE( ui_font_HollowPoint);
LV_FONT_DECLARE( ui_font_HollowPoint22);
LV_FONT_DECLARE( ui_font_HollowPoint32);


void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
