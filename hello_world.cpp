#include "pico/stdlib.h"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_explorer/pico_explorer.hpp"
#include "font8_data.hpp"

using namespace pimoroni;

ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

int main() {
    stdio_init_all();

    st7789.set_backlight(255);
    graphics.set_font(&font8);

    Pen BG    = graphics.create_pen(0, 32, 96);
    Pen WHITE = graphics.create_pen(255, 255, 255);

    while (true) {
        graphics.set_pen(BG);
        graphics.clear();

        graphics.set_pen(WHITE);
        graphics.text("Hello, World!", Point(20, 108), 200, 3.0f);

        st7789.update(&graphics);

        sleep_ms(100);
    }
}
