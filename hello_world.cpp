#include "pico/stdlib.h"
#include "pico_explorer.hpp"

using namespace pimoroni;

// 240x240 RGB565 framebuffer — lives in RAM
uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer display(buffer);

int main() {
    stdio_init_all();
    display.init();

    while (true) {
        // Dark blue background
        display.set_pen(0, 32, 96);
        display.clear();

        // White text, centred vertically
        display.set_pen(255, 255, 255);
        display.text("Hello, World!", Point(20, 105), 200, 4);

        display.update();
        sleep_ms(100);
    }
}
