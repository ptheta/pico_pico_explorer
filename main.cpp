#include "pico/stdlib.h"
#include "pico/time.h"
#include "display.hpp"
#include "button_manager.hpp"
#include "menu_screen.hpp"
#include "demo.hpp"
#include "pong_game.hpp"
#include "buzzer.hpp"

static constexpr uint32_t TICK_MS = 50;

enum class AppState { MENU, DEMO, PONG };

int main() {
    stdio_init_all();
    srand(time_us_32());
    Buzzer::init(0);   // GP0 → bridge to AUDIO header pin for piezo

    ButtonManager buttons;
    Display       display;
    MenuScreen    menu({"Bouncing Demo", "Pong"});
    Demo          demo;
    PongGame      pong;

    AppState state = AppState::MENU;
    display.show(menu);

    while (true) {
        absolute_time_t tick_end = make_timeout_time_ms(TICK_MS);

        buttons.update();
        const Buttons& btns = buttons.get();

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
        if (state != AppState::MENU && btns.gp1.pressed()) {
            if (state == AppState::DEMO) demo.reset();
            else if (state == AppState::PONG) { pong.reset(); Buzzer::stop(); }
            state = AppState::MENU;
            display.show(menu);
        }

        sleep_until(tick_end);
    }
}
