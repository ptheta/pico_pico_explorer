#include "graphic_screen.hpp"
#include <cstdlib>

static constexpr int SCREEN_W = 240;
static constexpr int SCREEN_H = 240;

GraphicScreen::GraphicScreen(int w, int h, Colour colour, int step_px, uint32_t anim_hz)
    : w_(w), h_(h), colour_(colour), step_(step_px), anim_hz_(anim_hz),
      x_(rand() % (SCREEN_W - w)),
      y_(rand() % (SCREEN_H - h)),
      dx_(rand() % 2 ? step_px : -step_px),
      dy_(rand() % 2 ? step_px : -step_px) {}

void GraphicScreen::render(Renderer& r) const {
    r.set_pen(colour_);
    r.fill_rect(x_, y_, w_, h_);
}

void GraphicScreen::animate() {
    x_ += dx_;
    y_ += dy_;

    if (x_ <= 0)            { x_ = 0;            dx_ =  step_; }
    if (x_ >= SCREEN_W - w_) { x_ = SCREEN_W - w_; dx_ = -step_; }
    if (y_ <= 0)            { y_ = 0;            dy_ =  step_; }
    if (y_ >= SCREEN_H - h_) { y_ = SCREEN_H - h_; dy_ = -step_; }
}

Rect GraphicScreen::bounds() const {
    return {x_, y_, w_, h_};
}

void GraphicScreen::on_collision(Screen&) {
    dx_ = -dx_;
    dy_ = -dy_;
}
