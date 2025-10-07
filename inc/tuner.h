
#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float freq_hz;        // measured frequency
    float magnitude;      // spectral magnitude at peak (normalized)
} tuner_estimate_t;

// Process one frame of int16 ADC samples (length n == FRAME_LEN) and estimate pitch (Hz).
// Returns 1 if estimate is valid; 0 otherwise (e.g., too quiet).
int tuner_estimate_frame(const int16_t* in, size_t n, tuner_estimate_t* out);

#ifdef __cplusplus
}
#endif
