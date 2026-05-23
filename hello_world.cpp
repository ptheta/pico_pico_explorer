#include "pico/stdlib.h"
#include "pico/time.h"
#include "display.hpp"
#include "text_screen.hpp"
#include <cstdlib>

int main() {
    stdio_init_all();
    srand(time_us_32());

    Display display;
    TextScreen screen("Hello, World!");
    display.show(screen);

    while (true) {
        sleep_ms(1000);
    }
}
