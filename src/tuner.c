
#include "tuner.h"
#include "goertzel.h"
#include "app_config.h"
#include <math.h>
#include <string.h>

static float g_winbuf[FRAME_LEN];
static goertzel_bin_t g_bins[(size_t)((GOERTZEL_F_MAX_HZ - GOERTZEL_F_MIN_HZ) / GOERTZEL_F_STEP_HZ) + 3];

int tuner_estimate_frame(const int16_t* in, size_t n, tuner_estimate_t* out) {
    if (n != FRAME_LEN) return 0;

    // Apply Hann window
    dsp_hann_window(in, g_winbuf, n);

    // Scan bins
    size_t bins_len = (size_t)((GOERTZEL_F_MAX_HZ - GOERTZEL_F_MIN_HZ) / GOERTZEL_F_STEP_HZ) + 1;
    size_t peak = goertzel_scan(g_winbuf, n, (float)SAMPLE_RATE_HZ, GOERTZEL_F_MIN_HZ, GOERTZEL_F_MAX_HZ, GOERTZEL_F_STEP_HZ, g_bins, bins_len);

    // Reject if too quiet
    float mag = g_bins[peak].magnitude;
    if (mag < 0.0008f) { // empirical noise floor threshold
        return 0;
    }

    // Interpolate for better estimate
    if (peak == 0 || peak+1 >= bins_len) return 0;
    float f_est = quad_interp_freq(g_bins, peak, GOERTZEL_F_STEP_HZ);

    out->freq_hz = f_est;
    out->magnitude = mag;
    return 1;
}
