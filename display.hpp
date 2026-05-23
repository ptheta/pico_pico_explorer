#pragma once

#include <memory>
#include <string_view>

class Display {
public:
    Display();
    ~Display();
    void show(std::string_view text);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
