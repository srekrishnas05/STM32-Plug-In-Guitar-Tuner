
#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float freq_hz;
    float magnitude;
} goertzel_bin_t;

// Apply Hann window in-place on int16 -> float buffer
void dsp_hann_window(const int16_t* in, float* out, size_t n);

// Compute Goertzel magnitude for a frequency at fs over frame x[]
float goertzel_mag(const float* x, size_t n, float fs, float f);

// Scan [fmin, fmax] (step fstep) and fill bins with magnitudes (bins_len = ceil((fmax-fmin)/fstep)+1). Returns index of max bin.
size_t goertzel_scan(const float* x, size_t n, float fs, float fmin, float fmax, float fstep, goertzel_bin_t* bins, size_t bins_len);

// Quadratic interpolation around peak index to estimate refined frequency (returns Hz). Requires bins[i-1], bins[i], bins[i+1] exist.
float quad_interp_freq(const goertzel_bin_t* bins, size_t peak_idx, float fstep);

// Utility
float fast_log2f(float x);

#ifdef __cplusplus
}
#endif
