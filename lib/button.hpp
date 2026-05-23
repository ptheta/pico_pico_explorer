#pragma once
#include "hardware/gpio.h"

class Button {
public:
    enum class Polarity { ActiveLow, ActiveHigh };

    Button(uint pin, Polarity polarity);

    void update();       // call once per tick before querying
    bool pressed() const; // true on the first tick the button goes down
    bool held() const;    // true while the button is held down

private:
    uint     pin_;
    Polarity polarity_;
    bool     cur_;
    bool     prev_;

    bool is_down(bool raw) const {
        return polarity_ == Polarity::ActiveLow ? !raw : raw;
    }
};
