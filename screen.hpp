#pragma once

class Renderer;

class Screen {
public:
    virtual ~Screen() = default;

    Screen()                           = default;
    Screen(const Screen&)              = delete;
    Screen& operator=(const Screen&)   = delete;
    Screen(Screen&&)                   = delete;
    Screen& operator=(Screen&&)        = delete;

    virtual void render(Renderer& r) const = 0;
};
