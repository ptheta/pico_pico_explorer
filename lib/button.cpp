#include "button.hpp"

Button::Button(uint pin, Polarity polarity)
    : pin_(pin), polarity_(polarity),
      cur_(polarity == Polarity::ActiveLow ? true : false),
      prev_(cur_)
{
    gpio_init(pin_);
    gpio_set_dir(pin_, GPIO_IN);
    if (polarity_ == Polarity::ActiveLow)
        gpio_pull_up(pin_);
    else
        gpio_pull_down(pin_);
}

void Button::update() {
    prev_ = cur_;
    cur_  = gpio_get(pin_);
}

bool Button::pressed() const {
    return is_down(cur_) && !is_down(prev_);
}

bool Button::held() const {
    return is_down(cur_);
}
