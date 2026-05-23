#include "display.hpp"
#include "renderer.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_explorer/pico_explorer.hpp"
#include "font8_data.hpp"
#include "pico/multicore.h"

#include <atomic>

using namespace pimoroni;

static constexpr uint32_t REFRESH_HZ  = 30;
static constexpr uint32_t FRAME_MS    = 1000 / REFRESH_HZ;

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
    ST7789                     st7789;
    PicoGraphics_PenRGB332     graphics;
    std::atomic<const Screen*> current_screen{nullptr};
    std::atomic<bool>          running{true};

    Impl()
        : st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT)),
          graphics(st7789.width, st7789.height, nullptr) {

        st7789.set_backlight(255);
        graphics.set_font(&font8);

        multicore_launch_core1(render_trampoline);
        multicore_fifo_push_blocking(reinterpret_cast<uintptr_t>(this));
    }

    ~Impl() {
        running.store(false, std::memory_order_relaxed);
    }

    void render_loop() {
        while (running.load(std::memory_order_relaxed)) {
            const Screen* screen = current_screen.load(std::memory_order_acquire);
            if (screen) {
                PicoRenderer renderer(graphics);
                screen->render(renderer);
                st7789.update(&graphics);
            }
            sleep_ms(FRAME_MS);
        }
    }

    static void render_trampoline() {
        auto* impl = reinterpret_cast<Impl*>(multicore_fifo_pop_blocking());
        impl->render_loop();
    }
};

Display::Display()  : impl(std::make_unique<Impl>()) {}
Display::~Display() = default;

void Display::show(const Screen& screen) {
    impl->current_screen.store(&screen, std::memory_order_release);
}
