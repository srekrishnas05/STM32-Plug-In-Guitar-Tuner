
#pragma once
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "app_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Call once after SPI and GPIOs are ready
void ssd1306_init(SPI_HandleTypeDef* hspi);

// Drawing primitives
void ssd1306_clear(void);
void ssd1306_update(void);
void ssd1306_draw_text_5x7(int x, int y, const char* s);
void ssd1306_draw_bar_centered(int y, int width, int height, int percent); // -100..+100 mapped

#ifdef __cplusplus
}
#endif
