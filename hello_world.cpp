#include "pico/stdlib.h"
#include "display.hpp"

int main() {
    stdio_init_all();

    Display display;

    while (true) {
        display.show("Hello, World!");
        sleep_ms(100);
    }
}
