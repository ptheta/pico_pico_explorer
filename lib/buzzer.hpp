#pragma once
#include <cstdint>

// Simple non-blocking PWM tone generator.
// Uses the RP2040 PWM hardware — once started, the tone plays autonomously.
// Call update() every frame to stop expired tones.
//
// Hardware note: on the Pico Explorer, bridge GP0 to the AUDIO header pin
// to drive the onboard piezo buzzer.
class Buzzer {
public:
    static void init(unsigned int gpio);
    static void play(unsigned int freq_hz, uint32_t duration_ms);
    static void update();   // stop tone when its duration has elapsed
    static void stop();     // silence immediately
};
