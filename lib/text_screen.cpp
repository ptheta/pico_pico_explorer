#include "text_screen.hpp"
#include "screen_dims.hpp"
#include <algorithm>
#include <cstdlib>

static constexpr int FONT_H_PX = 8;
static constexpr int WRAP      = 220;

TextScreen::TextScreen(std::string text, Colour fg,
                       float scale, int step_px, uint32_t anim_hz)
    : text_(std::move(text)), fg_(fg),
      scale_(scale), step_(step_px), anim_hz_(anim_hz),
      x_(SCREEN_W / 4),
      y_(SCREEN_H / 2 - static_cast<int>(FONT_H_PX * scale_) / 2),
      dx_(rand() % 2 ? step_px : -step_px),
      dy_(rand() % 2 ? step_px : -step_px) {}

void TextScreen::render(Renderer& r) const {
    if (text_w_ < 0)
        text_w_ = r.measure_text(text_, scale_);
    r.set_pen(fg_);
    r.text(text_, x_, y_, WRAP, scale_);
}

void TextScreen::animate() {
    x_ += dx_;
    y_ += dy_;

    int tw     = (text_w_ > 0) ? text_w_ : static_cast<int>(text_.length() * 5 * scale_);
    int x_max  = SCREEN_W - tw;
    int y_max  = SCREEN_H - static_cast<int>(FONT_H_PX * scale_);

    if (x_ <= 0)      { x_ = 0;      dx_ =  step_; }
    if (x_ >= x_max)  { x_ = x_max;  dx_ = -step_; }
    if (y_ <= 0)      { y_ = 0;      dy_ =  step_; }
    if (y_ >= y_max)  { y_ = y_max;  dy_ = -step_; }
}

Rect TextScreen::bounds() const {
    int tw = (text_w_ > 0) ? text_w_ : static_cast<int>(text_.length() * 5 * scale_);
    return {x_, y_, tw, static_cast<int>(FONT_H_PX * scale_)};
}

void TextScreen::on_collision(Screen&) {
    dx_ = -dx_;
    dy_ = -dy_;
}

void TextScreen::adjust_speed(int delta) {
    step_ = std::max(1, std::min(20, step_ + delta));
    dx_   = dx_ > 0 ?  step_ : -step_;
    dy_   = dy_ > 0 ?  step_ : -step_;
}

void TextScreen::reset() {
    step_   = DEFAULT_STEP_PX;
    x_      = SCREEN_W / 4;
    y_      = SCREEN_H / 2 - static_cast<int>(FONT_H_PX * scale_) / 2;
    dx_     = rand() % 2 ? step_ : -step_;
    dy_     = rand() % 2 ? step_ : -step_;
    text_w_ = -1;
}
