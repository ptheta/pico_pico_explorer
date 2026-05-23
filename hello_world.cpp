#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "display.hpp"
#include "scene.hpp"
#include "text_screen.hpp"
#include "graphic_screen.hpp"
#include "circle_screen.hpp"
#include "triangle_screen.hpp"
#include "menu_screen.hpp"
#include <memory>
#include <vector>
#include <cstdlib>

// Pico Explorer buttons — active-low, need pull-ups
static constexpr uint BTN_A = 12;  // speed up (demo) / navigate up (menu)
static constexpr uint BTN_B = 13;  // slow down (demo) / navigate down (menu)
static constexpr uint BTN_X = 14;  // add triangle (demo) / select (menu)
static constexpr uint BTN_Y = 15;  // short press: remove triangle  long press 3s: back to menu

static constexpr int Y_LONG_TICKS = 40; // 40 × 50 ms = 2 s

static void init_buttons() {
    for (uint pin : {BTN_A, BTN_B, BTN_X, BTN_Y}) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }
}

static uint8_t light_channel() { return static_cast<uint8_t>(150 + rand() % 106); }

enum class AppState { MENU, DEMO };

int main() {
    stdio_init_all();
    srand(time_us_32());
    init_buttons();

    Display display;

    MenuScreen menu({"Bouncing Demo"});

    TextScreen    text("Hello, World!");
    GraphicScreen box;
    CircleScreen  circle;
    Scene demo({0, 32, 96});
    demo.add(box);
    demo.add(circle);
    demo.add(text);

    std::vector<std::unique_ptr<TriangleScreen>> triangles;

    AppState state = AppState::MENU;
    display.show(menu);

    bool prev_a = true, prev_b = true, prev_x = true;
    int  y_hold_ticks    = 0;
    bool y_long_consumed = false;

    while (true) {
        bool a = gpio_get(BTN_A);
        bool b = gpio_get(BTN_B);
        bool x = gpio_get(BTN_X);
        bool y = gpio_get(BTN_Y);

        bool pressed_a = !a && prev_a;
        bool pressed_b = !b && prev_b;
        bool pressed_x = !x && prev_x;

        if (state == AppState::MENU) {
            if (pressed_a) menu.move_up();
            if (pressed_b) menu.move_down();
            if (pressed_x && menu.selected() == 0) {
                state = AppState::DEMO;
                display.show(demo);
            }
        } else {
            if (pressed_a) demo.adjust_speed(+1);
            if (pressed_b) demo.adjust_speed(-1);
            if (pressed_x) {
                Colour c = {light_channel(), light_channel(), light_channel()};
                auto t = std::make_unique<TriangleScreen>(TriangleScreen::DEFAULT_SIZE, c);
                demo.add(*t);
                triangles.push_back(std::move(t));
            }

            // Y: hold 2 s → back to menu; short press → remove triangle
            if (!y) {
                if (y_hold_ticks < Y_LONG_TICKS) ++y_hold_ticks;
                if (y_hold_ticks == Y_LONG_TICKS && !y_long_consumed) {
                    for (auto& t : triangles) demo.remove(*t);
                    triangles.clear();
                    demo.reset();
                    state = AppState::MENU;
                    display.show(menu);
                    y_long_consumed = true;
                }
            } else {
                if (!y_long_consumed && y_hold_ticks > 0 && !triangles.empty()) {
                    int idx = rand() % static_cast<int>(triangles.size());
                    demo.remove(*triangles[idx]);
                    triangles.erase(triangles.begin() + idx);
                }
                y_hold_ticks    = 0;
                y_long_consumed = false;
            }
        }

        prev_a = a; prev_b = b; prev_x = x;
        sleep_ms(50);
    }
}
