#include "buzzer.hpp"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/time.h"

static uint     g_slice;
static uint     g_channel;
static uint64_t g_stop_us = 0;

// Divider of 16 gives wrap values that fit in 16 bits for 220–4000 Hz.
static constexpr uint32_t SYS_CLOCK_HZ = 125'000'000;
static constexpr uint     CLK_DIV      = 16;

void Buzzer::init(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    gpio_set_drive_strength(gpio, GPIO_DRIVE_STRENGTH_12MA);
    g_slice   = pwm_gpio_to_slice_num(gpio);
    g_channel = pwm_gpio_to_channel(gpio);
    pwm_set_clkdiv(g_slice, static_cast<float>(CLK_DIV));
    pwm_set_enabled(g_slice, false);
}

void Buzzer::play(uint freq_hz, uint32_t duration_ms) {
    uint32_t wrap = SYS_CLOCK_HZ / CLK_DIV / freq_hz - 1;
    pwm_set_wrap(g_slice, wrap);
    pwm_set_chan_level(g_slice, g_channel, wrap / 2);   // 50% duty — square wave
    pwm_set_enabled(g_slice, true);
    g_stop_us = time_us_64() + static_cast<uint64_t>(duration_ms) * 1000;
}

void Buzzer::update() {
    if (g_stop_us > 0 && time_us_64() >= g_stop_us) {
        pwm_set_enabled(g_slice, false);
        g_stop_us = 0;
    }
}

void Buzzer::stop() {
    pwm_set_enabled(g_slice, false);
    g_stop_us = 0;
}
