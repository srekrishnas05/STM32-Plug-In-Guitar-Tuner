
# STM32 Plug-In Guitar Tuner (STM32F411, ADC+DMA, Goertzel, SSD1306 SPI)

**1/4″ jack → analog front end → ADC@8 kHz (DMA) → Goertzel scan → cents vs target → OLED.**  
Target: **STM32F411CE “Black Pill”**, **STM32CubeIDE/HAL**.

## TL;DR wiring (this repo’s defaults)
- **ADC in:** **PB1** → **ADC1 Channel 9 (ADC1_IN9)** with mid-bias.
- **Buttons:** **PC13** (tuning), **PB5** (string), internal pull-ups, EXTI falling-edge.
- **OLED (SSD1306 SPI1)**: **SCK=PA5**, **MOSI=PA7**, **DC=PB7**

If your pins differ, edit `Core/Inc/app_config.h` and/or remap in CubeMX.

---

## What the code does (high level)
1. **Sampling**
   - **TIM2** generates update events at **8 kHz** → ADC external trigger (TRGO).
   - **ADC1** samples **PB1 / Channel 9** (configured in `main.c`) in **DMA circular** mode into a **double buffer** (`FRAME_LEN*2`, FRAME_LEN = 1024).
   - HAL callbacks `HAL_ADC_ConvHalfCpltCallback` and `HAL_ADC_ConvCpltCallback` signal half/full frames.

2. **Preprocessing**
   - Convert **12-bit unsigned (0..4095)** to **int16 centered** by subtracting **2048**, scale to use int16 range.
   - Apply a **Hann window** to minimize spectral leakage.

3. **Pitch estimation**
   - **Goertzel scan** from **55..360 Hz** in **2 Hz** steps; pick the strongest bin.
   - **Quadratic interpolation** near the peak refines the estimate → **measured Hz**.

4. **Tuning logic**
   - Two buttons select **Tuning** (Standard, Drop D, Drop B, Drop C#, Drop A) and **String** (low→high).
   - We **lock to the selected string** and compute **cents** vs target:
     \[ \text{cents} = 1200 \log_2(f_\text{meas}/f_\text{target}) \]
   - OLED shows tuning name, string, target Hz, measured Hz, and a ±50‑cent bar.

5. **Display**
   - Minimal **SSD1306** SPI driver with a small 5×7 font.
   - 4‑wire hookup (SCK, MOSI, DC, VCC/GND). 
   - Update path: draw text/bar → `ssd1306_update()` pushes the framebuffer.

---

## Files & responsibilities
- `Core/Inc/app_config.h` — **pin map** (buttons, OLED, ADC), **ADC channel**, **tunings**, UI constants, debounce.
- `Core/Src/main.c` — HAL glue, **explicit ADC channel config (PB1/CH9)**, EXTI button callbacks, UI flow.
- `Core/Src/goertzel.c` + `Core/Inc/goertzel.h` — Hann window, Goertzel per-bin magnitude, scanning, interpolation.
- `Core/Src/tuner.c` + `Core/Inc/tuner.h` — `tuner_estimate_frame()` returning measured Hz + magnitude.
- `Core/Src/ssd1306_spi.c` + `Core/Inc/ssd1306_spi.h` — OLED driver (SPI1) with conditional **CS/RES** lines.
- `Core/Src/font5x7.c` + `Core/Inc/font5x7.h` — bitmap font.

---

## CubeMX checklist
- **RCC/Clocks:** 100 MHz core.
- **ADC1:** Enable ADC1; DMA (circular) enabled; external trigger = **TIM2 TRGO**; continuous conv **disabled**.
- **DMA:** ADC1→memory, circular, half/full interrupts.
- **TIM2:** Base timer update **8,000 Hz**; Master mode: **Update event (TRGO)**.
- **SPI1:** Master, mode 0, SCK=PA5, MOSI=PA7, baud prescaler /8 or /16. MISO unused.
- **GPIO:** PC13 & PB5 as `GPIO_EXTI` **Pull-Up**, **Falling edge**; PB7 as output (DC). If you later wire CS/RES, set those as outputs too.

---

## Tunings included (edit in `app_config.h`)
- **Standard**: E2 A2 D3 G3 B3 E4
- **Drop D**: D2 A2 D3 G3 B3 E4
- **Drop B**: B1 F#2 B2 E3 G#3 C#4
- **Drop C#**: C#2 G#2 C#3 F#3 A#3 D#4
- **Drop A**: A1 E2 A2 D3 F#3 B3

---

## Sampling & math details
- **Frame length**: 1024 @ 8 kHz → **128 ms** analysis window; updates each half-buffer (~128 ms cadence).
- **Hann window**: reduces leakage for non-integer periodic signals.
- **Goertzel vs FFT**: ~153 Goertzel bins (55..360 Hz, 2 Hz step) are lighter than a 1024‑pt FFT and sufficient for guitar.
- **Quadratic interpolation**: 3-bin parabola around peak → sub-bin smoothness.
- **Noise gate**: threshold ~**8e-4** on normalized magnitude in `tuner.c`.

---

## Porting tips
- Lower latency: set `FRAME_LEN=512`.
- Wider pitch range: tweak `GOERTZEL_F_MIN_HZ/MAX_HZ`.
- Different pins: edit macros in `app_config.h` + reconfigure in CubeMX as needed.
- Add CS/RES wires later: set `OLED_HAS_CS 1` / `OLED_HAS_RES 1` and map pins.

---

## Disclaimer
I used AI to help write the README, select parts for the analog front end (which is covered in depth in another page within this repo), and to debug code along with assisting in learning and implementing Geortzel.  
