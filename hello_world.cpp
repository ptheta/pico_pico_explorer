#include "pico/stdlib.h"
#include "pico/time.h"
#include "display.hpp"
#include "scene.hpp"
#include "text_screen.hpp"
#include "graphic_screen.hpp"
#include <cstdlib>

int main() {
    stdio_init_all();
    srand(time_us_32());

    Display display;

    TextScreen    text("Hello, World!");
    GraphicScreen box;

    Scene scene({0, 32, 96});
    scene.add(box);
    scene.add(text);

    display.show(scene);

    while (true) {
        sleep_ms(1000);
    }
}
