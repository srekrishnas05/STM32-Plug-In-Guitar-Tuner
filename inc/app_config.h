
#pragma once
#include "stm32f4xx_hal.h"

// ====== Sampling / DSP config ======
#define SAMPLE_RATE_HZ     8000U
#define FRAME_LEN          1024U        // samples per analysis frame (must match DMA half-buffer length)
#define GOERTZEL_F_MIN_HZ  55.0f        // extended for Drop A low string
#define GOERTZEL_F_MAX_HZ  360.0f
#define GOERTZEL_F_STEP_HZ 2.0f

// ====== UI config ======
#define OLED_WIDTH   128
#define OLED_HEIGHT  64
#define UI_BAR_WIDTH 100

// ====== Button GPIO mapping (edit only if your wiring differs) ======
// Buttons are active-low with internal pull-ups. EXTI falling-edge in CubeMX.
#define BTN_TUNING_GPIO_Port   GPIOC
#define BTN_TUNING_Pin         GPIO_PIN_13
#define BTN_STRING_GPIO_Port   GPIOB
#define BTN_STRING_Pin         GPIO_PIN_5

// Debounce (ms) for EXTI buttons
#define BTN_DEBOUNCE_MS 180U

// ====== OLED wiring (SSD1306 over SPI1) ======
// SPI1 uses SCK (PA5) and MOSI (PA7). MISO not used.
// We tie CS low on the board to make it a 4-wire hookup.
// We omit the RESET wire; use soft init instead.
#define OLED_HAS_CS   0   // 0 = CS tied low on the module (no CS GPIO used)
#define OLED_HAS_RES  0   // 0 = no RESET wire (soft init only)

// If you choose to wire CS or RES later, set these to 1 and map pins below.
#define OLED_CS_GPIO_Port   GPIOB
#define OLED_CS_Pin         GPIO_PIN_6
#define OLED_DC_GPIO_Port   GPIOB
#define OLED_DC_Pin         GPIO_PIN_7
#define OLED_RES_GPIO_Port  GPIOB
#define OLED_RES_Pin        GPIO_PIN_8

// ====== ADC input selection ======
// We explicitly configure ADC1 to use Channel 9 (PB1 = ADC1_IN9) in code
// before starting DMA, so you don't have to rely only on CubeMX.
// Leave CubeMX ADC1 enabled and DMA (circular) on.
#define ADC1_INPUT_GPIO_Port  GPIOB
#define ADC1_INPUT_Pin        GPIO_PIN_1
#define ADC1_INPUT_CHANNEL    ADC_CHANNEL_9   // PB1

// ====== Tuning tables ======
typedef struct { const char* name; float freq; } string_t;
typedef struct { const char* tuning_name; string_t strings[6]; } tuning_t;

// Frequencies (Hz) of notes used
#define NOTE_E2   82.4069f
#define NOTE_A2  110.0000f
#define NOTE_D3  146.832f
#define NOTE_G3  195.998f
#define NOTE_B3  246.942f
#define NOTE_E4  329.628f

#define NOTE_D2   73.4162f
#define NOTE_Cs2  69.2957f
#define NOTE_B1   61.7354f
#define NOTE_Fs2  92.4986f
#define NOTE_Cs3 138.591f
#define NOTE_E3  164.814f
#define NOTE_Gs3 207.652f
#define NOTE_Ds4 311.127f
#define NOTE_As3 233.082f
#define NOTE_A1   55.0000f

static const tuning_t kTunings[] = {
    { "Standard", {
        {"E2", NOTE_E2}, {"A2", NOTE_A2}, {"D3", NOTE_D3},
        {"G3", NOTE_G3}, {"B3", NOTE_B3}, {"E4", NOTE_E4},
    }},
    { "Drop D", {
        {"D2", NOTE_D2}, {"A2", NOTE_A2}, {"D3", NOTE_D3},
        {"G3", NOTE_G3}, {"B3", NOTE_B3}, {"E4", NOTE_E4},
    }},
    { "Drop B", {
        {"B1",  NOTE_B1}, {"F#2", NOTE_Fs2}, {"B2",  2.0f*NOTE_B1},
        {"E3",  NOTE_E3}, {"G#3", NOTE_Gs3}, {"C#4", NOTE_Cs3*2.0f},
    }},
    { "Drop C#", {
        {"C#2", NOTE_Cs2}, {"G#2", NOTE_Gs3/2.0f}, {"C#3", NOTE_Cs3},
        {"F#3", NOTE_Fs2*2.0f}, {"A#3", NOTE_As3}, {"D#4", NOTE_Ds4},
    }},
    { "Drop A", {
        {"A1", NOTE_A1}, {"E2", NOTE_E2}, {"A2", NOTE_A2},
        {"D3", NOTE_D3}, {"F#3", NOTE_Fs2*2.0f}, {"B3", NOTE_B3},
    }},
};
#define NUM_TUNINGS  (sizeof(kTunings)/sizeof(kTunings[0]))
