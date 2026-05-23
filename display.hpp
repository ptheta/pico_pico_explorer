#pragma once

#include <string_view>
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_explorer/pico_explorer.hpp"

class Display {
public:
    Display();
    void show(std::string_view text);

private:
    pimoroni::ST7789 st7789;
    pimoroni::PicoGraphics_PenRGB332 graphics;
    pimoroni::Pen bg;
    pimoroni::Pen fg;
};
