#include "triangle_screen.hpp"
#include "screen_dims.hpp"
#include <algorithm>
#include <cstdlib>

TriangleScreen::TriangleScreen(int size, Colour colour, int step_px, uint32_t anim_hz)
    : half_(size / 2), colour_(colour), step_(step_px), anim_hz_(anim_hz),
      cx_(half_ + rand() % (SCREEN_W - size)),
      cy_(half_ + rand() % (SCREEN_H - size)),
      dx_(rand() % 2 ? step_px : -step_px),
      dy_(rand() % 2 ? step_px : -step_px) {}

void TriangleScreen::render(Renderer& r) const {
    r.set_pen(colour_);
    // Apex top-centre, base bottom-left and bottom-right
    r.fill_triangle(cx_,          cy_ - half_,
                    cx_ - half_,  cy_ + half_,
                    cx_ + half_,  cy_ + half_);
}

void TriangleScreen::animate() {
    cx_ += dx_;
    cy_ += dy_;

    if (cx_ <= half_)            { cx_ = half_;            dx_ =  step_; }
    if (cx_ >= SCREEN_W - half_) { cx_ = SCREEN_W - half_; dx_ = -step_; }
    if (cy_ <= half_)            { cy_ = half_;             dy_ =  step_; }
    if (cy_ >= SCREEN_H - half_) { cy_ = SCREEN_H - half_; dy_ = -step_; }
}

Rect TriangleScreen::bounds() const {
    return {cx_ - half_, cy_ - half_, 2 * half_, 2 * half_};
}

void TriangleScreen::on_collision(Screen&) {
    dx_ = -dx_;
    dy_ = -dy_;
}

void TriangleScreen::adjust_speed(int delta) {
    step_ = std::max(1, std::min(20, step_ + delta));
    dx_   = dx_ > 0 ?  step_ : -step_;
    dy_   = dy_ > 0 ?  step_ : -step_;
}

void TriangleScreen::reset() {
    step_ = DEFAULT_STEP_PX;
    cx_   = half_ + rand() % (SCREEN_W - 2 * half_);
    cy_   = half_ + rand() % (SCREEN_H - 2 * half_);
    dx_   = rand() % 2 ?  step_ : -step_;
    dy_   = rand() % 2 ?  step_ : -step_;
}
