#include "ui/ui.h"

#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>


#define TFT_CS   10 //34 //     10 or 34 (FSPI CS0) 
#define TFT_MOSI 11 //35 //     11 or 35 (FSPI D)
#define TFT_SCLK 12 //36 //     12 or 36 (
#define TFT_MISO 13 //37 //     13 or 37 (FSPI Q)
#define TFT_DC    8
#define TFT_RST   3

void showSpeedAnimation();

class LGFX : public lgfx::LGFX_Device
{
lgfx::Panel_ST7789     _panel_instance;
lgfx::Bus_SPI       _bus_instance;   // SPIバスのインスタンス
lgfx::Light_PWM     _light_instance;

public:
  LGFX(void)
  {
    { // バス制御の設定を行います。
      auto cfg = _bus_instance.config();    // バス設定用の構造体を取得します。
      cfg.spi_host = SPI2_HOST;     // 使用するSPIを選択  (VSPI_HOST or HSPI_HOST)
      cfg.spi_mode = 0;             // SPI通信モードを設定 (0 ~ 3)
      cfg.freq_write = 80000000;    // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
      cfg.freq_read  = 16000000;    // 受信時のSPIクロック
      cfg.spi_3wire  = false;        // 受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock   = true;        // トランザクションロックを使用する場合はtrueを設定
      cfg.dma_channel = 1;          // Set the DMA channel (1 or 2. 0=disable)   使用するDMAチャンネルを設定 (0=DMA不使用)
      cfg.pin_sclk = TFT_SCLK;            // SPIのSCLKピン番号を設定
      cfg.pin_mosi = TFT_MOSI;            // SPIのMOSIピン番号を設定
      cfg.pin_miso = TFT_MISO;            // SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc   = TFT_DC;            // SPIのD/Cピン番号を設定  (-1 = disable)
      _bus_instance.config(cfg);    // 設定値をバスに反映します。
      _panel_instance.setBus(&_bus_instance);      // バスをパネルにセットします。
    }

    { // 表示パネル制御の設定を行います。
      auto cfg = _panel_instance.config();    // 表示パネル設定用の構造体を取得します。
      cfg.pin_cs           =    TFT_CS;  // CSが接続されているピン番号   (-1 = disable)
      cfg.pin_rst          =    TFT_RST;  // RSTが接続されているピン番号  (-1 = disable)
      cfg.pin_busy         =    -1;  // BUSYが接続されているピン番号 (-1 = disable)
      cfg.memory_width     =   240;  // ドライバICがサポートしている最大の幅
      cfg.memory_height    =   320;  // ドライバICがサポートしている最大の高さ
      cfg.panel_width      =   240;  // 実際に表示可能な幅
      cfg.panel_height     =   320;  // 実際に表示可能な高さ
      cfg.offset_x         =     0;  // パネルのX方向オフセット量
      cfg.offset_y         =     0;  // パネルのY方向オフセット量
      cfg.offset_rotation  =     0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel =     8;  // ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits  =     1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable         =  true;  // データ読出しが可能な場合 trueに設定
      cfg.invert           = true;  // パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order        = false;  // パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit       = false;  // データ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared       =  true;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }
    
    { // バックライト制御の設定を行います。（必要なければ削除）
      auto cfg = _light_instance.config();    // バックライト設定用の構造体を取得します。

      cfg.pin_bl = 12;              // バックライトが接続されているピン番号
      cfg.invert = false;           // バックライトの輝度を反転させる場合 true
      cfg.freq   = 44100;           // バックライトのPWM周波数
      cfg.pwm_channel = 7;          // 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
    }

    setPanel(&_panel_instance); // 使用するパネルをセットします。
  }
};

LGFX tft;

/*Change to your screen resolution*/
static const uint32_t screenWidth  = 240;
static const uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

unsigned long refreshLcdTs = 0;

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
   uint32_t w = ( area->x2 - area->x1 + 1 );
   uint32_t h = ( area->y2 - area->y1 + 1 );

   tft.startWrite();
   tft.setAddrWindow( area->x1, area->y1, w, h );
   //tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
   tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
   tft.endWrite();

   lv_disp_flush_ready( disp );
}


void initLcd()
{
   tft.begin();        
   tft.setBrightness(255);
  
   lv_init();
   lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

   /*Initialize the display*/
   static lv_disp_drv_t disp_drv;
   lv_disp_drv_init(&disp_drv);

   /*Change the following line to your display resolution*/
   disp_drv.hor_res = screenWidth;
   disp_drv.ver_res = screenHeight;
   disp_drv.sw_rotate = 1;
   disp_drv.rotated = LV_DISP_ROT_270;
   disp_drv.flush_cb = my_disp_flush;
   disp_drv.draw_buf = &draw_buf;
   lv_disp_drv_register(&disp_drv);

   ui_init();

    showSpeedAnimation();
}

void showSpeedAnimation() {
    char data[10];
    for (int i = 0; i <= 20; i ++) {
      sprintf(data, "%d", i);
      lv_label_set_text(ui_speedValue,data);
      lv_bar_set_value(ui_speedBar,i * 5 ,LV_ANIM_OFF);
      lv_timer_handler(); /* let the GUI do its work */
    }

        for (int i = 20; i >= 0; i --) {

      sprintf(data, "%d", i);
      lv_label_set_text(ui_speedValue,data);
      lv_bar_set_value(ui_speedBar,i * 5 ,LV_ANIM_OFF);
      lv_timer_handler(); /* let the GUI do its work */
    }
}

void refreshLcd(unsigned long now) {
  if ((now - refreshLcdTs) < 25) {
    return;
  }
  refreshLcdTs = now;
  lv_timer_handler(); /* let the GUI do its work */
}