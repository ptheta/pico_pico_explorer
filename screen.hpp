#pragma once

#include <cstdint>

class Renderer;

class Screen {
public:
    virtual ~Screen() = default;

    Screen()                           = default;
    Screen(const Screen&)              = delete;
    Screen& operator=(const Screen&)   = delete;
    Screen(Screen&&)                   = delete;
    Screen& operator=(Screen&&)        = delete;

    virtual void render(Renderer& r) const = 0;

    // Called by Display at animation_hz() per second. animation_hz() must be
    // a divisor of the display refresh rate (30 Hz), or 0 for no animation.
    virtual void animate() {}
    virtual uint32_t animation_hz() const { return 0; }
};
