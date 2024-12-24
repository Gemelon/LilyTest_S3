#include <stdio.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_io_i80.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_dma_utils.h"
#include <misc/lv_types.h>
#include "lvgl.h"
#include "./lv_conf.h"
#include "src/drivers/display/st7789/lv_st7789.h"
#include "ui.h"

#define USE_LILYGO_T_Display_S3 1
#define USE_Waveshare_ESP32_S3_1_47inch 0

#if USE_LILYGO_T_Display_S3
#define LCD_H_RES 170
#define LCD_V_RES 320
#endif

#if USE_Waveshare_ESP32_S3_1_47inch
#define LCD_H_RES 170
#define LCD_V_RES 320
#endif

#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)

#define BOARD_TFT_BL (38)
#define BOARD_TFT_DATA0 (39)
#define BOARD_TFT_DATA1 (40)
#define BOARD_TFT_DATA2 (41)
#define BOARD_TFT_DATA3 (42)
#define BOARD_TFT_DATA4 (45)
#define BOARD_TFT_DATA5 (46)
#define BOARD_TFT_DATA6 (47)
#define BOARD_TFT_DATA7 (48)
#define BOARD_TFT_RST (5)
#define BOARD_TFT_CS (6)
#define BOARD_TFT_DC (7)
#define BOARD_TFT_WR (8)
#define BOARD_TFT_RD (9)

#define RAMWR (0x2C) // RAMWR (2Ch): Memory Write

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
static uint8_t buf1[LCD_H_RES * LCD_V_RES / 10 * BYTES_PER_PIXEL];

static const char *TAG = "TFT";

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;

lv_display_t *lcd_disp;

typedef struct
{
    uint32_t addr;
    uint8_t param[20];
    uint32_t len;
} lcd_cmd_t;

// APKLVSR ESP32-S3 T-Display Modul 1.9 Zoll 7789 ESP32 Display TTGO
// LILYGO T-Display-S3 ESP32-S3 1.9 Zoll ST7789 LCD Display TTGO
#if USE_LILYGO_T_Display_S3
// lcd_cmd_t lcd_st7789v[] = {
//     {0x11, {0}, 0 | 0x80}, // SLPOUT (11h): Sleep Out
//     {0x36, {0x04}, 1},
//     {0x3A, {0X05}, 1},                         // COLMOD (3Ah): Interface Pixel Format ‘101’ = 16bit/pixel
//     {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5}, // PORCTRL (B2h): Porch Setting {BPA, FPA, PSEN, BPB/FPB, BPC/FPC}
//     {0xB7, {0X75}, 1},                         // GCTRL (B7h): Gate Control
//     {0xBB, {0X28}, 1},                         // VCOMS (BBh): VCOM Setting 28h = 1.1V
//     {0xC0, {0X2C}, 1},                         // LCMCTRL (C0h): LCM Control
//     {0xC2, {0X01}, 1},                         // VDVVRHEN (C2h): VDV and VRH Command Enable
//     {0xC3, {0X1F}, 1},                         // VRHS (C3h): VRH Set 5.1+( vcom+vcom offset+vdv)
//     {0xC6, {0X13}, 1},                         // FRCTRL2 (C6h): Frame Rate Control in Normal Mode 53Hz
//     {0xD0, {0XA7}, 1},                         // PWCTRL1 (D0h): Power Control 1
//     {0xD0, {0XA4, 0XA1}, 2},
//     {0xD6, {0XA1}, 1},
//     {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14}, // PVGAMCTRL (E0h): Positive Voltage Gamma Control
//     {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14}, // NVGAMCTRL (E1h): Negative Voltage Gamma Control
// };

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};

#endif

// Waveshare ESP32-S3 1.47inch LCD Display Development Board, 72×320, 262K Color
#if USE_Waveshare_ESP32_S3_1_47inch
lcd_cmd_t lcd_st7789v[] = {
    {0x36, {0x00}, 1},
    {0x3A, {0X55}, 1},                                                                                /* Interface Pixel Format, 16bits/pixel for RGB/MCU interface */
    {0xB0, {0x00, 0xE8}, 2},                                                                          /* RAMCTRL RAM Control */
    {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5},                                                        /* Porch Setting */
    {0xB7, {0x75}, 1},                                                                                /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    {0xBB, {0x1A}, 1},                                                                                /* VCOM Setting, VCOM=1.175V */
    {0xC0, {0x80}, 1},                                                                                /* LCM Control, XOR: BGR, MX, MH */
    {0xC2, {0x01, 0xff}, 2},                                                                          /* VDV and VRH Command Enable, enable=1 */
    {0xC3, {0x13}, 1},                                                                                /* VRH Set, Vap=4.4+... */
    {0xC4, {0x20}, 1},                                                                                /* VDV Set, VDV=0 */
    {0xC6, {0x0F}, 1},                                                                                /* Frame Rate Control, 60Hz, inversion=0 */
    {0xD0, {0xA4, 0xA1}, 1},                                                                          /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    {0xE0, {0xD0, 0x0D, 0x14, 0x0D, 0x0D, 0x09, 0x38, 0x44, 0x4E, 0x3A, 0x17, 0x18, 0x2F, 0x30}, 14}, /* Positive Voltage Gamma Control */
    {0xE1, {0xD0, 0x09, 0x0F, 0x08, 0x07, 0x14, 0x37, 0x44, 0x4D, 0x38, 0x15, 0x16, 0x2C, 0x2E}, 14}, /* Negative Voltage Gamma Control */
    {0x21, {0}, 0},                                                                                   /* Sleep Out */
    {0x29, {0}, 0},                                                                                   /* Display On */
    {0x2C, {0}, 0},                                                                                   /* RAMWR Memory Write */
};
#endif

void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

bool display_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    // lv_display_t *disp_driver = (lv_display_t *)user_ctx;
    lv_display_flush_ready(lcd_disp);
    return false;
}

void display_init()
{
    ESP_LOGI(TAG, "============T-Display-S3============");

    gpio_config_t rd_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << BOARD_TFT_RD};
    ESP_ERROR_CHECK(gpio_config(&rd_gpio_config));
    gpio_set_level(BOARD_TFT_RD, 1);

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = BOARD_TFT_DC,
        .wr_gpio_num = BOARD_TFT_WR,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .data_gpio_nums =
            {
                BOARD_TFT_DATA0,
                BOARD_TFT_DATA1,
                BOARD_TFT_DATA2,
                BOARD_TFT_DATA3,
                BOARD_TFT_DATA4,
                BOARD_TFT_DATA5,
                BOARD_TFT_DATA6,
                BOARD_TFT_DATA7,
            },
        .bus_width = 8,
        .max_transfer_bytes = LCD_H_RES * LCD_V_RES * sizeof(uint16_t),
        .psram_trans_align = 64,
        .sram_trans_align = 4};
    esp_lcd_new_i80_bus(&bus_config, &i80_bus);

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = BOARD_TFT_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_levels =
            {
                .dc_idle_level = 0,
                .dc_cmd_level = 0,
                .dc_dummy_level = 0,
                .dc_data_level = 1,
            },
        .on_color_trans_done = display_notify_lvgl_flush_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BOARD_TFT_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
        .vendor_config = NULL};
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
}

void screenFulsh(lv_display_t *display, const lv_area_t *area, uint8_t *color_p)
{
    lv_display_flush_ready(lcd_disp);
}

static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    // ESP_LOGI(TAG, "lcd_send_cmd: CMD 0x%x, cmd_size %i, param_size %i", *cmd, (int)cmd_size, (int)param_size);
    LV_UNUSED(disp);
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, *cmd, param, param_size));
    if (param_size & 0x80)
        vTaskDelay(pdMS_TO_TICKS(120));
}

static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    // ESP_LOGI(TAG, "lcd_send_color: CMD 0x%x, cmd_size %i, param_size %i", *cmd, (int)cmd_size, (int)param_size);
    LV_UNUSED(disp);
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_color(io_handle, *cmd, param, param_size));
}

void LVGL_Init(void)
{
    ESP_LOGI(TAG, "LVGL_Init");

    /********************* LVGL *********************/
    // The LVGL tick timer has to by initialized bfore lv_init() is called.
    // The ESP will constantly throw an watchdog error if not.
    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"};

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    esp_lcd_panel_reset(panel_handle);

    esp_lcd_panel_init(panel_handle);

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << BOARD_TFT_BL};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(BOARD_TFT_BL, 1);

    // Transfare the initialisation sequenz to the LCD panel
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++)
    {
        esp_lcd_panel_io_tx_param(io_handle, lcd_st7789v[i].addr, lcd_st7789v[i].param, lcd_st7789v[i].len & 0x7f);
        if (lcd_st7789v[i].len & 0x80)
            vTaskDelay(pdMS_TO_TICKS(120));
    }

    // Must by called before all other lv funktions.
    lv_init();
    lcd_disp = lv_st7789_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, lcd_send_cmd, lcd_send_color);

    // This funktion is the V9 substitute for the V8 funktion 'esp_lcd_panel_invert_color(panel_handle, false);'
    lv_st7789_set_invert(lcd_disp, true);
    
    lv_display_set_buffers(lcd_disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_offset(lcd_disp, 35, 0);
    lv_display_set_color_format(lcd_disp, LV_COLOR_FORMAT_RGB565);
    lv_display_rotation_t rotation = LV_DISPLAY_ROTATION_0;
    lv_display_set_rotation(lcd_disp, rotation);

    esp_lcd_panel_disp_on_off(panel_handle, true);
}

void app_main(void)
{
    display_init();
    LVGL_Init();
    ui_init();

    while (1)
    {
        ui_tick();

        vTaskDelay(pdMS_TO_TICKS(10));

        lv_timer_handler();
    }
}