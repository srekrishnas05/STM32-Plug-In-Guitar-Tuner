
#include "goertzel.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void dsp_hann_window(const int16_t* in, float* out, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        float w = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * ((float)i) / (float)(n - 1)));
        out[i] = (float)in[i] * w;
    }
}

float goertzel_mag(const float* x, size_t n, float fs, float f) {
    float k = (f / fs) * n;
    float w = 2.0f * (float)M_PI * k / (float)n;
    float coeff = 2.0f * cosf(w);
    float s0 = 0.0f, s1 = 0.0f, s2 = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        s0 = x[i] + coeff * s1 - s2;
        s2 = s1;
        s1 = s0;
    }
    float real = s1 - s2 * cosf(w);
    float imag = s2 * sinf(w);
    return sqrtf(real*real + imag*imag) / (float)n;
}

size_t goertzel_scan(const float* x, size_t n, float fs, float fmin, float fmax, float fstep, goertzel_bin_t* bins, size_t bins_len) {
    size_t idx = 0;
    float best = -1.0f;
    size_t best_i = 0;
    for (float f = fmin; f <= fmax + 0.0001f && idx < bins_len; f += fstep, ++idx) {
        float m = goertzel_mag(x, n, fs, f);
        bins[idx].freq_hz = f;
        bins[idx].magnitude = m;
        if (m > best) { best = m; best_i = idx; }
    }
    return best_i;
}

float quad_interp_freq(const goertzel_bin_t* bins, size_t peak_idx, float fstep) {
    if (peak_idx == 0) return bins[peak_idx].freq_hz;
    float y1 = bins[peak_idx-1].magnitude;
    float y2 = bins[peak_idx].magnitude;
    float y3 = bins[peak_idx+1].magnitude;
    float denom = (2.0f*(y1 - 2.0f*y2 + y3));
    float delta = 0.0f;
    if (fabsf(denom) > 1e-12f) {
        delta = (y1 - y3) / denom; // in bins, around [-0.5, 0.5]
    }
    return bins[peak_idx].freq_hz + delta * fstep;
}

// Fast log2f via natural log (good enough here)
float fast_log2f(float x) {
    return logf(x) / logf(2.0f);
}
