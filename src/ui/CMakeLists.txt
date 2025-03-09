SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_guage_bg_png.c
    images/ui_img_guage_indicator_png.c
    images/ui_img_compas_png.c
    images/ui_img_devider_png.c
    images/ui_img_guage_bg_1_png.c
    images/ui_img_battery_bg_png.c
    images/ui_img_temperature_png.c
    images/ui_img_light23_png.c
    images/ui_img_range_bg_png.c
    images/ui_img_range_knob_png.c
    images/ui_img_clk2_png.c
    images/ui_img_left_arr_png.c
    images/ui_img_gps_png.c
    fonts/ui_font_HollowPoint.c
    fonts/ui_font_HollowPoint22.c
    fonts/ui_font_HollowPoint32.c)

add_library(ui ${SOURCES})
