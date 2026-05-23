#include "pico/stdlib.h"
#include "display.hpp"
#include "text_screen.hpp"

int main() {
    stdio_init_all();

    Display display;
    TextScreen screen("Hello, World!");
    display.show(screen);

    while (true) {
        sleep_ms(1000);
    }
}
