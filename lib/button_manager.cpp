#include "button_manager.hpp"

ButtonManager::ButtonManager()
    : a_  (12, Button::Polarity::ActiveLow),
      b_  (13, Button::Polarity::ActiveLow),
      x_  (14, Button::Polarity::ActiveLow),
      y_  (15, Button::Polarity::ActiveLow),
      gp1_( 1, Button::Polarity::ActiveHigh),
      gp2_( 2, Button::Polarity::ActiveHigh),
      btns_{a_, b_, x_, y_, gp1_, gp2_}
{}

void ButtonManager::update() {
    a_.update(); b_.update(); x_.update(); y_.update();
    gp1_.update(); gp2_.update();
}
