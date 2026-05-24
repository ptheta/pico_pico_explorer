#pragma once

#include <cstdint>
#include <vector>
#include "pico/critical_section.h"
#include "screen.hpp"
#include "renderer.hpp"

class Scene : public Screen {
public:
    explicit Scene(Colour bg = {0, 0, 0}, uint32_t display_hz = 30);

    void add(Screen& screen);
    void remove(Screen& screen);

    void     render(Renderer& r) const override;
    void     animate()                   override;
    uint32_t animation_hz()        const override { return display_hz_; }
    void     adjust_speed(int delta)     override;
    void     reset()                     override;

private:
    struct Entry {
        Screen*  screen;
        uint32_t frame_counter = 0;
    };

    struct CollisionPair {
        Screen* a;
        Screen* b;
        bool operator==(const CollisionPair& o) const { return a == o.a && b == o.b; }
    };

    struct PendingOp {
        enum class Kind { Add, Remove };
        Kind    kind;
        Screen* screen;
    };

    void do_remove(Screen& screen); // must be called under cs_

    Colour                       bg_;
    uint32_t                     display_hz_;
    std::vector<Entry>           entries_;
    std::vector<CollisionPair>   active_collisions_;
    std::vector<CollisionPair>   current_collisions_;
    std::vector<PendingOp>       pending_;
    bool                         in_animate_ = false;
    mutable critical_section_t   cs_;
};
