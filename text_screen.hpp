#pragma once

#include <string>
#include "screen.hpp"
#include "renderer.hpp"

class TextScreen : public Screen {
public:
    explicit TextScreen(std::string text,
                        Colour bg = {0, 32, 96},
                        Colour fg = {255, 255, 255});

    void render(Renderer& r) const override;

private:
    std::string text_;
    Colour bg_;
    Colour fg_;
};
