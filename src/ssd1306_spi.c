
#include "ssd1306_spi.h"
#include "font5x7.h"
#include <string.h>

static SPI_HandleTypeDef* g_hspi;
static uint8_t g_fb[OLED_WIDTH * OLED_HEIGHT / 8];

static void dc_low()  { HAL_GPIO_WritePin(OLED_DC_GPIO_Port,  OLED_DC_Pin,  GPIO_PIN_RESET); }
static void dc_high() { HAL_GPIO_WritePin(OLED_DC_GPIO_Port,  OLED_DC_Pin,  GPIO_PIN_SET); }

#if OLED_HAS_CS
static void cs_low()  { HAL_GPIO_WritePin(OLED_CS_GPIO_Port,  OLED_CS_Pin,  GPIO_PIN_RESET); }
static void cs_high() { HAL_GPIO_WritePin(OLED_CS_GPIO_Port,  OLED_CS_Pin,  GPIO_PIN_SET); }
#else
static void cs_low()  {}
static void cs_high() {}
#endif

#if OLED_HAS_RES
static void res_low() { HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET); }
static void res_high(){ HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET); }
#endif

static void cmd(uint8_t c) {
    cs_low(); dc_low();
    HAL_SPI_Transmit(g_hspi, &c, 1, HAL_MAX_DELAY);
    cs_high();
}
static void data(const uint8_t* d, size_t n) {
    cs_low(); dc_high();
    HAL_SPI_Transmit(g_hspi, (uint8_t*)d, n, HAL_MAX_DELAY);
    cs_high();
}

void ssd1306_init(SPI_HandleTypeDef* hspi) {
    g_hspi = hspi;

#if OLED_HAS_RES
    // Hard reset if wired
    res_low(); HAL_Delay(10); res_high(); HAL_Delay(10);
#endif
    // Soft init sequence
    cmd(0xAE); // display off
    cmd(0xD5); cmd(0x80);
    cmd(0xA8); cmd(0x3F);
    cmd(0xD3); cmd(0x00);
    cmd(0x40);
    cmd(0x8D); cmd(0x14); // charge pump
    cmd(0x20); cmd(0x00); // horizontal addressing
    cmd(0xA1);
    cmd(0xC8);
    cmd(0xDA); cmd(0x12);
    cmd(0x81); cmd(0x7F);
    cmd(0xD9); cmd(0xF1);
    cmd(0xDB); cmd(0x40);
    cmd(0xA4);
    cmd(0xA6);
    ssd1306_clear();
    ssd1306_update();
    cmd(0xAF); // display on
}

void ssd1306_clear(void) {
    memset(g_fb, 0x00, sizeof(g_fb));
}

static void set_pixel(int x, int y, int on) {
    if (x<0||x>=OLED_WIDTH||y<0||y>=OLED_HEIGHT) return;
    if (on) g_fb[(y>>3)*OLED_WIDTH + x] |=  (1<<(y&7));
    else    g_fb[(y>>3)*OLED_WIDTH + x] &= ~(1<<(y&7));
}

void ssd1306_draw_text_5x7(int x, int y, const char* s) {
    while (*s) {
        unsigned char c = (unsigned char)(*s);
        if (c < 0x20 || c > 0x7F) c = 0x20;
        const uint8_t* glyph = font5x7[c - 0x20];
        for (int col=0; col<5; ++col) {
            for (int row=0; row<7; ++row) {
                int on = (glyph[col] >> row) & 1;
                set_pixel(x+col, y+row, on);
            }
        }
        // 1px space
        for (int row=0; row<7; ++row) set_pixel(x+5, y+row, 0);
        x += 6;
    }
}

void ssd1306_draw_bar_centered(int y, int width, int height, int percent) {
    if (percent < -100) percent = -100;
    if (percent >  100) percent =  100;
    int cx = OLED_WIDTH/2;
    int half = width/2;
    // Outline
    for (int i=0;i<width;i++) { set_pixel(cx-half+i, y, 1); set_pixel(cx-half+i, y+height-1, 1); }
    for (int j=0;j<height;j++) { set_pixel(cx-half, y+j, 1); set_pixel(cx+half-1, y+j, 1); }
    // Fill toward left/right
    int fill = (percent * half) / 100;
    for (int i=0;i<half;i++) {
        int on_left  = (i <= -fill) && (fill<0);
        int on_right = (i <  fill) && (fill>0);
        for (int j=1;j<height-1;j++) {
            set_pixel(cx-1 - i, y+j, on_left);
            set_pixel(cx + i,   y+j, on_right);
        }
    }
}

void ssd1306_update(void) {
    cmd(0x21); cmd(0); cmd(OLED_WIDTH-1); // columns
    cmd(0x22); cmd(0); cmd((OLED_HEIGHT/8)-1); // pages
    data(g_fb, sizeof(g_fb));
}
