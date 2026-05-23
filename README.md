# pico_exp1

A playground repo for the Raspberry Pi Pico W + [Pimoroni Pico Explorer](https://shop.pimoroni.com/products/pico-explorer-base) board, written in C++ and built hand-in-hand with [Claude](https://claude.ai) as a collaborative coding experiment.

The goal isn't production code — it's a space to explore embedded C++ ideas, try things out, and see what emerges when you pair a human with an AI on a small hardware project.

## Hardware

- Raspberry Pi Pico W
- Pimoroni Pico Explorer Base (240×240 ST7789 display, 4 buttons, breadboard)
- Raspberry Pi Debug Probe (for flashing via SWD — no BOOTSEL juggling)

## What it does

Boots to a menu. From the menu you can launch a bouncing demo where a rectangle, circle, and text all bounce around the screen, collide with each other, and react. While in the demo:

| Button | Action |
|--------|--------|
| A | Speed up |
| B | Slow down |
| X | Add a triangle (random colour) |
| Y (short press) | Remove a random triangle |
| Y (hold 2s) | Back to menu |

## Architecture

The code is structured around a small rendering abstraction:

- **`Screen`** — base class for anything that can be rendered and animated
- **`Renderer`** — interface that hides PicoGraphics behind plain draw calls
- **`Scene`** — composite screen: owns a list of child screens, clears once, renders all children, handles per-child animation timing and collision detection
- **`Display`** — PIMPL class that runs the 30 Hz refresh loop on core 1, accepts a `Screen&` to display

All Pimoroni/PicoGraphics headers are confined to `display.cpp`. The rest of the code has no hardware dependencies.

```mermaid
classDiagram
    class Screen {
        <<abstract>>
        +render(Renderer&)*
        +animate()
        +animation_hz() uint32_t
        +bounds() Rect
        +on_collision(Screen&)
        +adjust_speed(int)
        +reset()
    }

    class Renderer {
        <<abstract>>
        +set_pen(Colour)
        +clear()
        +fill_rect(int,int,int,int)
        +fill_circle(int,int,int)
        +fill_triangle(int,int,int,int,int,int)
        +text(string_view,int,int,int,float)
        +measure_text(string_view,float) int
    }

    class PicoRenderer {
        -graphics PicoGraphics_PenRGB332
    }

    class Display {
        +show(Screen&)
        -impl Impl
        note "Runs render loop on core 1"
    }

    class Scene {
        +add(Screen&)
        +remove(Screen&)
        +adjust_speed(int)
        +reset()
        -entries_ vector
        -cs_ critical_section_t
    }

    class MenuScreen {
        +move_up()
        +move_down()
        +selected() int
        -selected_ atomic~int~
    }

    class TextScreen
    class GraphicScreen
    class CircleScreen
    class TriangleScreen

    Screen <|-- Scene
    Screen <|-- TextScreen
    Screen <|-- GraphicScreen
    Screen <|-- CircleScreen
    Screen <|-- TriangleScreen
    Screen <|-- MenuScreen

    Renderer <|-- PicoRenderer

    Scene "1" o-- "0..*" Screen : children

    Display *-- PicoRenderer : owns
    Display ..> Screen : shows

    Screen ..> Renderer : uses in render()
```

## Building

One-time setup (Fedora — installs toolchain, clones SDKs, builds picotool):

```bash
bash setup.sh
```

Incremental builds:

```bash
cmake --build build -j$(nproc)
```

## Flashing

Connect the Raspberry Pi Debug Probe to the Pico W's SWD port, then:

```bash
bash flash.sh
```

No BOOTSEL button required.

## Project structure

```
display.cpp/hpp        — Display class (PIMPL, core 1 render loop)
renderer.hpp           — Abstract Renderer interface
screen.hpp             — Abstract Screen base class + Rect
scene.cpp/hpp          — Composite scene with collision detection
text_screen.cpp/hpp    — Bouncing text
graphic_screen.cpp/hpp — Bouncing rectangle
circle_screen.cpp/hpp  — Bouncing circle
triangle_screen.cpp/hpp— Bouncing triangle (dynamically added/removed)
menu_screen.cpp/hpp    — Simple menu with keyboard navigation
screen_dims.hpp        — Shared display dimensions
hello_world.cpp        — main(): app state machine and button handling
```
