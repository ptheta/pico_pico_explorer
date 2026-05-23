#pragma once

#include <cstdint>
#include <string_view>

struct Colour {
    uint8_t r, g, b;
};

class Renderer {
public:
    virtual ~Renderer() = default;

    virtual void set_pen(Colour c) = 0;
    virtual void clear() = 0;
    virtual void fill_rect(int x, int y, int w, int h) = 0;
    virtual void text(std::string_view t, int x, int y, int wrap, float scale) = 0;
};
