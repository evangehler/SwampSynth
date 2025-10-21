#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <math.h>

#define CLOCK_PIN 13
#define CV_PIN    14
#define SYS_CLK   125000000.0f

#define MV_PER_HZ 0.00100f   // 440 Hz ≈ 560 mV
#define CURVATURE  -0.000000005f
#define V_SUPPLY  3.3f

// A4 = 440 Hz reference
#define A4_FREQ 440.0f
#define A4_MIDI 69   // MIDI note number for A4

void set_pwm_freq(uint gpio, float freq_hz) {
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint chan  = pwm_gpio_to_channel(gpio);

    float clkdiv = 1.0f;
    uint32_t wrap = (uint32_t)((SYS_CLK / (freq_hz * clkdiv)) - 1.0f);
    if (wrap > 65535) {
        clkdiv = (SYS_CLK / (freq_hz * 65536.0f));
        wrap   = 65535;
    }
    pwm_set_clkdiv(slice, clkdiv);
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, chan, wrap / 2);
}

int main() {
    stdio_init_all();

    // --- CLOCK PWM ---
    gpio_set_function(CLOCK_PIN, GPIO_FUNC_PWM);
    uint slice_clk = pwm_gpio_to_slice_num(CLOCK_PIN);
    pwm_set_enabled(slice_clk, true);

    // --- CV PWM (~100 kHz carrier) ---
    gpio_set_function(CV_PIN, GPIO_FUNC_PWM);
    uint slice_cv = pwm_gpio_to_slice_num(CV_PIN);
    const float pwm_cv_freq = 100000.0f;
    float clkdiv_cv = 1.0f;
    uint32_t wrap_cv = (uint32_t)((SYS_CLK / (pwm_cv_freq * clkdiv_cv)) - 1.0f);
    pwm_set_clkdiv(slice_cv, clkdiv_cv);
    pwm_set_wrap(slice_cv, wrap_cv);
    pwm_set_enabled(slice_cv, true);

    const int low_note  = 48;   // C1
    const int high_note = 48;   // C2
    const int note_time_ms = 100; // 0.25 s per note

    while (true) {
        // Ascending chromatic
        for (int midi = low_note; midi <= high_note; midi++) {
            float freq = A4_FREQ * powf(2.0f, (midi - A4_MIDI) / 12.0f);

            set_pwm_freq(CLOCK_PIN, freq);

            float v_cv = MV_PER_HZ * freq + CURVATURE * freq * freq;
            if (v_cv < 0) v_cv = 0;
            if (v_cv > V_SUPPLY) v_cv = V_SUPPLY;
            float duty = v_cv / V_SUPPLY;
           
            pwm_set_chan_level(slice_cv, PWM_CHAN_A, (uint32_t)(wrap_cv * duty));

              printf("MIDI %d  |  %.3f Hz  |  Vcv = %.4f V  |  Duty = %.2f%%\n",
                   midi, freq, v_cv, duty * 100.0f);

            sleep_ms(note_time_ms);
        }

        // Descending chromatic
        for (int midi = high_note; midi >= low_note; midi--) {
            float freq = A4_FREQ * powf(2.0f, (midi - A4_MIDI) / 12.0f);

            set_pwm_freq(CLOCK_PIN, freq);

            float v_cv = MV_PER_HZ * freq;
            if (v_cv > V_SUPPLY) v_cv = V_SUPPLY;
            float duty = v_cv / V_SUPPLY;
            pwm_set_chan_level(slice_cv, PWM_CHAN_A, (uint32_t)(wrap_cv * duty));

              printf("MIDI %d  |  %.3f Hz  |  Vcv = %.4f V  |  Duty = %.2f%%\n",
                   midi, freq, v_cv, duty * 100.0f);


            sleep_ms(note_time_ms);
        }
    }
}
