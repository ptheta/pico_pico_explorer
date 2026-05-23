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
static constexpr uint BTN_A   = 12;
static constexpr uint BTN_B   = 13;
static constexpr uint BTN_X   = 14;
static constexpr uint BTN_Y   = 15;
// Extra buttons — active-high, need pull-downs
static constexpr uint BTN_GP1 =  1;  // back to menu
static constexpr uint BTN_GP2 =  2;  // select

static void init_buttons() {
    for (uint pin : {BTN_A, BTN_B, BTN_X, BTN_Y}) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }
    for (uint pin : {BTN_GP1, BTN_GP2}) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_down(pin);
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

    bool prev_a = true, prev_b = true, prev_x = true, prev_y = true;
    bool prev_gp1 = false, prev_gp2 = false;

    while (true) {
        bool a   = gpio_get(BTN_A);
        bool b   = gpio_get(BTN_B);
        bool x   = gpio_get(BTN_X);
        bool y   = gpio_get(BTN_Y);
        bool gp1 = gpio_get(BTN_GP1);
        bool gp2 = gpio_get(BTN_GP2);

        bool pressed_a   = !a   && prev_a;
        bool pressed_b   = !b   && prev_b;
        bool pressed_x   = !x   && prev_x;
        bool pressed_y   = !y   && prev_y;
        bool pressed_gp1 = gp1  && !prev_gp1;
        bool pressed_gp2 = gp2  && !prev_gp2;

        if (state == AppState::MENU) {
            if (pressed_a || pressed_x) menu.move_up();
            if (pressed_b || pressed_y) menu.move_down();
            if (pressed_gp2) {
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
            if (pressed_y && !triangles.empty()) {
                int idx = rand() % static_cast<int>(triangles.size());
                demo.remove(*triangles[idx]);
                triangles.erase(triangles.begin() + idx);
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
                if (!y) pong.move_right(+1);  // Y held = right paddle down
            }
        }

        // GP1: instant back to menu from any non-menu state
        if (state != AppState::MENU && pressed_gp1) {
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
        }

        prev_a = a; prev_b = b; prev_x = x; prev_y = y; prev_gp1 = gp1; prev_gp2 = gp2;
        sleep_ms(50);
    }
}
