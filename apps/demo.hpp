#pragma once

#include "screen.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "text_screen.hpp"
#include "graphic_screen.hpp"
#include "circle_screen.hpp"
#include "triangle_screen.hpp"
#include "button.hpp"
#include <memory>
#include <vector>

class Demo : public Screen {
public:
    Demo();

    void     render(Renderer& r) const override;
    void     animate()                 override;
    uint32_t animation_hz()      const override;
    void     adjust_speed(int delta)   override;
    void     reset()                   override;

    void handle_buttons(const Buttons& btns);

    void add_triangle();     // add a random-coloured triangle
    void remove_triangle();  // remove a random triangle (no-op if none)

private:
    TextScreen    text_;
    GraphicScreen box_;
    CircleScreen  circle_;
    Scene         scene_;
    std::vector<std::unique_ptr<TriangleScreen>> triangles_;
};
