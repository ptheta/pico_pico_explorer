#pragma once

#include <cstdint>

class Renderer;

struct Rect {
    int x, y, w, h;

    bool intersects(const Rect& other) const {
        return x < other.x + other.w && x + w > other.x &&
               y < other.y + other.h && y + h > other.y;
    }
};

class Screen {
public:
    virtual ~Screen() = default;

    Screen()                           = default;
    Screen(const Screen&)              = delete;
    Screen& operator=(const Screen&)   = delete;
    Screen(Screen&&)                   = delete;
    Screen& operator=(Screen&&)        = delete;

    virtual void render(Renderer& r) const = 0;

    // Called by Scene at animation_hz() per second. Must be a divisor of the
    // display refresh rate (30 Hz), or 0 for no animation.
    virtual void     animate()                   {}
    virtual uint32_t animation_hz()        const { return 0; }
    virtual Rect     bounds()              const { return {0, 0, 0, 0}; }
    virtual void     on_collision(Screen&)       {}
    virtual void     adjust_speed(int)           {}
    virtual void     reset()                     {}
};
