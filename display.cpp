#include "display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_explorer/pico_explorer.hpp"
#include "font8_data.hpp"

using namespace pimoroni;

struct Display::Impl {
    ST7789 st7789;
    PicoGraphics_PenRGB332 graphics;
    Pen bg;
    Pen fg;

    Impl()
        : st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT)),
          graphics(st7789.width, st7789.height, nullptr),
          bg(graphics.create_pen(0, 32, 96)),
          fg(graphics.create_pen(255, 255, 255)) {

        st7789.set_backlight(255);
        graphics.set_font(&font8);
    }
};

Display::Display() : impl(std::make_unique<Impl>()) {}

Display::~Display() = default;

void Display::show(std::string_view text) {
    impl->graphics.set_pen(impl->bg);
    impl->graphics.clear();

    impl->graphics.set_pen(impl->fg);
    impl->graphics.text(text, Point(20, 108), 200, 3.0f);

    impl->st7789.update(&impl->graphics);
}
