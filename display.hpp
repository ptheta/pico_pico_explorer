#pragma once

#include <memory>
#include "screen.hpp"

class Display {
public:
    Display();
    ~Display();

    Display(const Display&)            = delete;
    Display& operator=(const Display&) = delete;
    Display(Display&&)                 = delete;
    Display& operator=(Display&&)      = delete;

    void show(Screen& screen);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
