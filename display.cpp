#include "display.hpp"
#include "renderer.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_explorer/pico_explorer.hpp"
#include "font8_data.hpp"

using namespace pimoroni;

struct PicoRenderer : Renderer {
    PicoGraphics_PenRGB332& graphics;

    explicit PicoRenderer(PicoGraphics_PenRGB332& g) : graphics(g) {}

    void set_pen(Colour c) override {
        graphics.set_pen(graphics.create_pen(c.r, c.g, c.b));
    }

    void clear() override {
        graphics.clear();
    }

    void text(std::string_view t, int x, int y, int wrap, float scale) override {
        graphics.text(t, Point(x, y), wrap, scale);
    }
};

struct Display::Impl {
    ST7789 st7789;
    PicoGraphics_PenRGB332 graphics;

    Impl()
        : st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT)),
          graphics(st7789.width, st7789.height, nullptr) {

        st7789.set_backlight(255);
        graphics.set_font(&font8);
    }
};

Display::Display() : impl(std::make_unique<Impl>()) {}

Display::~Display() = default;

void Display::show(const Screen& screen) {
    PicoRenderer renderer(impl->graphics);
    screen.render(renderer);
    impl->st7789.update(&impl->graphics);
}
