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
#include "pong_game.hpp"
#include "buzzer.hpp"
#include <memory>
#include <vector>
#include <cstdlib>

// Pico Explorer buttons — active-low, need pull-ups
static constexpr uint BTN_A = 12;
static constexpr uint BTN_B = 13;
static constexpr uint BTN_X = 14;
static constexpr uint BTN_Y = 15;

// Hold Y for this many 50 ms ticks to exit back to menu
static constexpr int Y_LONG_TICKS = 100; // 100 × 50 ms = 5 s

static void init_buttons() {
    for (uint pin : {BTN_A, BTN_B, BTN_X, BTN_Y}) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }
}

static uint8_t light_channel() { return static_cast<uint8_t>(150 + rand() % 106); }

enum class AppState { MENU, DEMO, PONG };

int main() {
    stdio_init_all();
    srand(time_us_32());
    init_buttons();
    Buzzer::init(0);   // GP0 → bridge to AUDIO header pin for piezo

    Display display;

    MenuScreen menu({"Bouncing Demo", "Pong"});

    // --- Bouncing demo ---
    TextScreen    text("Hello, World!");
    GraphicScreen box;
    CircleScreen  circle;
    Scene demo({0, 32, 96});
    demo.add(box);
    demo.add(circle);
    demo.add(text);
    std::vector<std::unique_ptr<TriangleScreen>> triangles;

    // --- Pong ---
    PongGame pong;

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
            if (pressed_x) {
                switch (menu.selected()) {
                    case 0: state = AppState::DEMO; display.show(demo); break;
                    case 1: state = AppState::PONG; display.show(pong); break;
                }
            }

        } else if (state == AppState::DEMO) {
            if (pressed_a) demo.adjust_speed(+1);
            if (pressed_b) demo.adjust_speed(-1);
            if (pressed_x) {
                Colour c = {light_channel(), light_channel(), light_channel()};
                auto t = std::make_unique<TriangleScreen>(TriangleScreen::DEFAULT_SIZE, c);
                demo.add(*t);
                triangles.push_back(std::move(t));
            }

        } else if (state == AppState::PONG) {
            if (pong.is_finished()) {
                pong.reset();
                Buzzer::stop();
                state = AppState::MENU;
                display.show(menu);
            } else {
                if (!a) pong.move_left(-1);   // A held = left paddle up
                if (!b) pong.move_left(+1);   // B held = left paddle down
                if (!x) pong.move_right(-1);  // X held = right paddle up
                if (!y) pong.move_right(+1);  // Y held = right paddle down (+ long press below)
            }
        }

        // Y long press: exit to menu from any non-menu state
        // Short press in demo: remove a triangle
        if (state != AppState::MENU) {
            if (!y) {
                if (y_hold_ticks < Y_LONG_TICKS) ++y_hold_ticks;
                if (y_hold_ticks == Y_LONG_TICKS && !y_long_consumed) {
                    if (state == AppState::DEMO) {
                        for (auto& t : triangles) demo.remove(*t);
                        triangles.clear();
                        demo.reset();
                    } else if (state == AppState::PONG) {
                        pong.reset();
                        Buzzer::stop();
                    }
                    state = AppState::MENU;
                    display.show(menu);
                    y_long_consumed = true;
                }
            } else {
                if (!y_long_consumed && y_hold_ticks > 0 && state == AppState::DEMO
                        && !triangles.empty()) {
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
