#include "text_screen.hpp"

TextScreen::TextScreen(std::string text, Colour bg, Colour fg)
    : text_(std::move(text)), bg_(bg), fg_(fg) {}

void TextScreen::render(Renderer& r) const {
    r.set_pen(bg_);
    r.clear();
    r.set_pen(fg_);
    r.text(text_, 20, 108, 200, 3.0f);
}
