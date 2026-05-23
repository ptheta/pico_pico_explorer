#include "text_screen.hpp"
#include <cstdlib>

static constexpr int SCREEN_W    = 240;
static constexpr int SCREEN_H    = 240;
static constexpr int FONT_W_PX   = 6;  // font8 character width in pixels at scale 1
static constexpr int FONT_H_PX   = 8;  // font8 character height in pixels at scale 1
static constexpr int WRAP        = 220;

TextScreen::TextScreen(std::string text, Colour bg, Colour fg,
                       float scale, int step_px, uint32_t anim_hz)
    : text_(std::move(text)), bg_(bg), fg_(fg),
      scale_(scale), step_(step_px), anim_hz_(anim_hz),
      x_(SCREEN_W / 4),
      y_(SCREEN_H / 2 - static_cast<int>(FONT_H_PX * scale_) / 2),
      dx_(rand() % 2 ? step_px : -step_px),
      dy_(rand() % 2 ? step_px : -step_px),
      x_max_(SCREEN_W - static_cast<int>(text_.length() * FONT_W_PX * scale_)),
      y_max_(SCREEN_H - static_cast<int>(FONT_H_PX * scale_)) {}

void TextScreen::animate() {
    x_ += dx_;
    y_ += dy_;

    if (x_ <= 0)       { x_ = 0;       dx_ =  step_; }
    if (x_ >= x_max_)  { x_ = x_max_;  dx_ = -step_; }
    if (y_ <= 0)       { y_ = 0;       dy_ =  step_; }
    if (y_ >= y_max_)  { y_ = y_max_;  dy_ = -step_; }
}

void TextScreen::render(Renderer& r) const {
    r.set_pen(bg_);
    r.clear();
    r.set_pen(fg_);
    r.text(text_, x_, y_, WRAP, scale_);
}
