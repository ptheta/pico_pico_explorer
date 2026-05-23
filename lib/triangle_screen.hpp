#pragma once

#include "screen.hpp"
#include "renderer.hpp"

class TriangleScreen : public Screen {
public:
    static constexpr int      DEFAULT_SIZE    = 40;
    static constexpr int      DEFAULT_STEP_PX = 5;
    static constexpr uint32_t DEFAULT_ANIM_HZ = 30;

    explicit TriangleScreen(int      size    = DEFAULT_SIZE,
                            Colour   colour  = {255, 255, 255},
                            int      step_px = DEFAULT_STEP_PX,
                            uint32_t anim_hz = DEFAULT_ANIM_HZ);

    void     render(Renderer& r) const override;
    void     animate()                   override;
    uint32_t animation_hz()        const override { return anim_hz_; }
    Rect     bounds()              const override;
    void     on_collision(Screen&)       override;
    void     adjust_speed(int delta)     override;
    void     reset()                     override;

private:
    int      half_;
    Colour   colour_;
    int      step_;
    uint32_t anim_hz_;
    int      cx_, cy_, dx_, dy_;
};
