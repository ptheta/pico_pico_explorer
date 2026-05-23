#include "pico/stdlib.h"
#include "pico/time.h"
#include "display.hpp"
#include "scene.hpp"
#include "text_screen.hpp"
#include "graphic_screen.hpp"
#include "circle_screen.hpp"
#include "triangle_screen.hpp"
#include "menu_screen.hpp"
#include "pong_game.hpp"
#include "buzzer.hpp"
#include "button.hpp"
#include <memory>
#include <vector>
#include <cstdlib>

static uint8_t light_channel() { return static_cast<uint8_t>(150 + rand() % 106); }

enum class AppState { MENU, DEMO, PONG };

int main() {
    stdio_init_all();
    srand(time_us_32());
    Buzzer::init(0);   // GP0 → bridge to AUDIO header pin for piezo

    Button btn_a  (12, Button::Polarity::ActiveLow);
    Button btn_b  (13, Button::Polarity::ActiveLow);
    Button btn_x  (14, Button::Polarity::ActiveLow);
    Button btn_y  (15, Button::Polarity::ActiveLow);
    Button btn_gp1( 1, Button::Polarity::ActiveHigh);  // back to menu
    Button btn_gp2( 2, Button::Polarity::ActiveHigh);  // select

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

    while (true) {
        btn_a.update(); btn_b.update(); btn_x.update(); btn_y.update();
        btn_gp1.update(); btn_gp2.update();

        if (state == AppState::MENU) {
            if (btn_a.pressed() || btn_x.pressed()) menu.move_up();
            if (btn_b.pressed() || btn_y.pressed()) menu.move_down();
            if (btn_gp2.pressed()) {
                switch (menu.selected()) {
                    case 0: state = AppState::DEMO; display.show(demo); break;
                    case 1: state = AppState::PONG; display.show(pong); break;
                }
            }

        } else if (state == AppState::DEMO) {
            if (btn_a.pressed()) demo.adjust_speed(+1);
            if (btn_b.pressed()) demo.adjust_speed(-1);
            if (btn_x.pressed()) {
                Colour c = {light_channel(), light_channel(), light_channel()};
                auto t = std::make_unique<TriangleScreen>(TriangleScreen::DEFAULT_SIZE, c);
                demo.add(*t);
                triangles.push_back(std::move(t));
            }
            if (btn_y.pressed() && !triangles.empty()) {
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
                if (btn_a.held()) pong.move_left(-1);   // left paddle up
                if (btn_b.held()) pong.move_left(+1);   // left paddle down
                if (btn_x.held()) pong.move_right(-1);  // right paddle up
                if (btn_y.held()) pong.move_right(+1);  // right paddle down
            }
        }

        // GP1: instant back to menu from any non-menu state
        if (state != AppState::MENU && btn_gp1.pressed()) {
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

        sleep_ms(50);
    }
}
