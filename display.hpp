#pragma once

#include <memory>
#include <string_view>

class Display {
public:
    Display();
    ~Display();

    Display(const Display&)            = delete;
    Display& operator=(const Display&) = delete;
    Display(Display&&)                 = delete;
    Display& operator=(Display&&)      = delete;

    void show(std::string_view text);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
