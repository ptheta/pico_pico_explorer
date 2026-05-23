#pragma once

#include <atomic>
#include <string>
#include <vector>
#include "screen.hpp"
#include "renderer.hpp"
#include "button.hpp"

class MenuScreen : public Screen {
public:
    explicit MenuScreen(std::vector<std::string> items,
                        Colour fg        = {200, 200, 200},
                        Colour highlight = {255, 200, 0},
                        Colour bg        = {0, 0, 64});

    void render(Renderer& r) const override;

    void handle_buttons(const Buttons& btns);
    bool take_activation();  // returns true once when user selects, then resets

    void move_up();
    void move_down();
    int  selected() const;

private:
    std::vector<std::string> items_;
    Colour                   fg_;
    Colour                   highlight_;
    Colour                   bg_;
    std::atomic<int>         selected_{0};
    bool                     activated_ = false;
};
