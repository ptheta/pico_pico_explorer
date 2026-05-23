#include "menu_screen.hpp"

static constexpr float SCALE  = 2.0f;
static constexpr int   LINE_H = 24;    // 8px font * scale 2 = 16px + 8px gap
static constexpr int   WRAP   = 220;

MenuScreen::MenuScreen(std::vector<std::string> items, Colour fg, Colour highlight, Colour bg)
    : items_(std::move(items)), fg_(fg), highlight_(highlight), bg_(bg) {}

void MenuScreen::render(Renderer& r) const {
    r.set_pen(bg_);
    r.clear();

    r.set_pen(fg_);
    r.text("MENU", 10, 10, WRAP, SCALE);

    int sel = selected_.load(std::memory_order_relaxed);
    int y   = 50;
    for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
        if (i == sel) {
            r.set_pen(highlight_);
            r.fill_rect(5, y - 2, 230, LINE_H - 4);
            r.set_pen(bg_);
        } else {
            r.set_pen(fg_);
        }
        r.text(items_[i], 10, y, WRAP, SCALE);
        y += LINE_H;
    }
}

void MenuScreen::move_up() {
    int cur = selected_.load(std::memory_order_relaxed);
    if (cur > 0) selected_.store(cur - 1, std::memory_order_relaxed);
}

void MenuScreen::move_down() {
    int cur = selected_.load(std::memory_order_relaxed);
    if (cur < static_cast<int>(items_.size()) - 1)
        selected_.store(cur + 1, std::memory_order_relaxed);
}

int MenuScreen::selected() const {
    return selected_.load(std::memory_order_relaxed);
}
