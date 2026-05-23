#pragma once

#include "screen.hpp"
#include "renderer.hpp"

class GraphicScreen : public Screen {
public:
    static constexpr int      DEFAULT_W       = 40;
    static constexpr int      DEFAULT_H       = 40;
    static constexpr int      DEFAULT_STEP_PX = 3;
    static constexpr uint32_t DEFAULT_ANIM_HZ = 10;

    explicit GraphicScreen(int      w       = DEFAULT_W,
                           int      h       = DEFAULT_H,
                           Colour   colour  = {255, 140, 0},
                           int      step_px = DEFAULT_STEP_PX,
                           uint32_t anim_hz = DEFAULT_ANIM_HZ);

    void     render(Renderer& r) const override;
    void     animate()                   override;
    uint32_t animation_hz()        const override { return anim_hz_; }
    Rect     bounds()              const override;
    void     on_collision(Screen&)       override;

private:
    int      w_, h_;
    Colour   colour_;
    int      step_;
    uint32_t anim_hz_;
    int      x_, y_, dx_, dy_;
};
