#pragma once
#include "button.hpp"

class ButtonManager {
public:
    ButtonManager();

    void           update();
    const Buttons& get() const { return btns_; }

    ButtonManager(const ButtonManager&)            = delete;
    ButtonManager& operator=(const ButtonManager&) = delete;

private:
    Button  a_, b_, x_, y_, gp1_, gp2_;
    Buttons btns_;
};
