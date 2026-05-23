#pragma once

#include <string>
#include "screen.hpp"
#include "renderer.hpp"

class TextScreen : public Screen {
public:
    static constexpr float    DEFAULT_SCALE   = 2.0f;
    static constexpr int      DEFAULT_STEP_PX = 3;
    static constexpr uint32_t DEFAULT_ANIM_HZ = 10;

    explicit TextScreen(std::string text,
                        Colour      fg       = {255, 255, 255},
                        float       scale    = DEFAULT_SCALE,
                        int         step_px  = DEFAULT_STEP_PX,
                        uint32_t    anim_hz  = DEFAULT_ANIM_HZ);

    void     render(Renderer& r) const override;
    void     animate()                   override;
    uint32_t animation_hz()        const override { return anim_hz_; }
    Rect     bounds()              const override;
    void     on_collision(Screen&)       override;

private:
    std::string text_;
    Colour      fg_;
    float       scale_;
    int         step_;
    uint32_t    anim_hz_;
    int         x_, y_, dx_, dy_;
    int         x_max_, y_max_;
};
