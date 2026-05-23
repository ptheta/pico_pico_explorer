#include "display.hpp"
#include "font8_data.hpp"

using namespace pimoroni;

Display::Display()
    : st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT)),
      graphics(st7789.width, st7789.height, nullptr),
      bg(graphics.create_pen(0, 32, 96)),
      fg(graphics.create_pen(255, 255, 255)) {

    st7789.set_backlight(255);
    graphics.set_font(&font8);
}

void Display::show(std::string_view text) {
    graphics.set_pen(bg);
    graphics.clear();

    graphics.set_pen(fg);
    graphics.text(text, Point(20, 108), 200, 3.0f);

    st7789.update(&graphics);
}
