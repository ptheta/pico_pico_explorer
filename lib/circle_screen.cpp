#include "circle_screen.hpp"
#include "screen_dims.hpp"
#include <algorithm>
#include <cstdlib>

CircleScreen::CircleScreen(int radius, Colour colour, int step_px, uint32_t anim_hz)
    : r_(radius), colour_(colour), step_(step_px), anim_hz_(anim_hz),
      cx_(radius + rand() % (SCREEN_W - 2 * radius)),
      cy_(radius + rand() % (SCREEN_H - 2 * radius)),
      dx_(rand() % 2 ? step_px : -step_px),
      dy_(rand() % 2 ? step_px : -step_px) {}

void CircleScreen::render(Renderer& r) const {
    r.set_pen(colour_);
    r.fill_circle(cx_, cy_, r_);
}

void CircleScreen::animate() {
    cx_ += dx_;
    cy_ += dy_;

    if (cx_ <= r_)            { cx_ = r_;            dx_ =  step_; }
    if (cx_ >= SCREEN_W - r_) { cx_ = SCREEN_W - r_; dx_ = -step_; }
    if (cy_ <= r_)            { cy_ = r_;             dy_ =  step_; }
    if (cy_ >= SCREEN_H - r_) { cy_ = SCREEN_H - r_; dy_ = -step_; }
}

Rect CircleScreen::bounds() const {
    return {cx_ - r_, cy_ - r_, 2 * r_, 2 * r_};
}

void CircleScreen::on_collision(Screen&) {
    dx_ = -dx_;
    dy_ = -dy_;
}

void CircleScreen::adjust_speed(int delta) {
    step_ = std::max(1, std::min(20, step_ + delta));
    dx_   = dx_ > 0 ?  step_ : -step_;
    dy_   = dy_ > 0 ?  step_ : -step_;
}

void CircleScreen::reset() {
    step_ = DEFAULT_STEP_PX;
    cx_   = r_ + rand() % (SCREEN_W - 2 * r_);
    cy_   = r_ + rand() % (SCREEN_H - 2 * r_);
    dx_   = rand() % 2 ? step_ : -step_;
    dy_   = rand() % 2 ? step_ : -step_;
}
