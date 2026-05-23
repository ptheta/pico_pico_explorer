#include "pico/stdlib.h"
#include "pico/time.h"
#include "display.hpp"
#include "menu_screen.hpp"
#include "demo.hpp"
#include "pong_game.hpp"
#include "buzzer.hpp"
#include "button.hpp"

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

    Buttons btns{btn_a, btn_b, btn_x, btn_y, btn_gp1, btn_gp2};

    Display    display;
    MenuScreen menu({"Bouncing Demo", "Pong"});
    Demo       demo;
    PongGame   pong;

    AppState state = AppState::MENU;
    display.show(menu);

    while (true) {
        btn_a.update(); btn_b.update(); btn_x.update(); btn_y.update();
        btn_gp1.update(); btn_gp2.update();

        if (state == AppState::MENU) {
            menu.handle_buttons(btns);
            if (menu.take_activation()) {
                switch (menu.selected()) {
                    case 0: state = AppState::DEMO; display.show(demo); break;
                    case 1: state = AppState::PONG; display.show(pong); break;
                }
            }

        } else if (state == AppState::DEMO) {
            demo.handle_buttons(btns);

        } else if (state == AppState::PONG) {
            pong.handle_buttons(btns);
            if (pong.is_finished()) {
                pong.reset();
                Buzzer::stop();
                state = AppState::MENU;
                display.show(menu);
            }
        }

        // GP1: system-wide back to menu
        if (state != AppState::MENU && btn_gp1.pressed()) {
            if (state == AppState::DEMO) demo.reset();
            else if (state == AppState::PONG) { pong.reset(); Buzzer::stop(); }
            state = AppState::MENU;
            display.show(menu);
        }

        sleep_ms(50);
    }
}
