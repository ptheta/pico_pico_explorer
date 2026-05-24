#include "scene.hpp"
#include "pico/multicore.h"
#include <algorithm>

Scene::Scene(Colour bg, uint32_t display_hz)
    : bg_(bg), display_hz_(display_hz) {
    critical_section_init(&cs_);
}

void Scene::add(Screen& screen) {
    if (get_core_num() == 1 && in_animate_) {
        pending_.push_back({PendingOp::Kind::Add, &screen});
        return;
    }
    critical_section_enter_blocking(&cs_);
    entries_.push_back({&screen, 0});
    critical_section_exit(&cs_);
}

void Scene::remove(Screen& screen) {
    if (get_core_num() == 1 && in_animate_) {
        pending_.push_back({PendingOp::Kind::Remove, &screen});
        return;
    }
    critical_section_enter_blocking(&cs_);
    do_remove(screen);
    critical_section_exit(&cs_);
}

void Scene::do_remove(Screen& screen) {
    auto it = std::find_if(entries_.begin(), entries_.end(),
                           [&](const Entry& e) { return e.screen == &screen; });
    if (it != entries_.end()) entries_.erase(it);
    active_collisions_.erase(
        std::remove_if(active_collisions_.begin(), active_collisions_.end(),
                       [&](const CollisionPair& p) {
                           return p.a == &screen || p.b == &screen;
                       }),
        active_collisions_.end());
}

void Scene::render(Renderer& r) const {
    critical_section_enter_blocking(&cs_);
    r.set_pen(bg_);
    r.clear();
    for (const auto& entry : entries_)
        entry.screen->render(r);
    critical_section_exit(&cs_);
}

void Scene::animate() {
    critical_section_enter_blocking(&cs_);

    in_animate_ = true;

    for (auto& entry : entries_) {
        uint32_t hz = entry.screen->animation_hz();
        if (hz > 0 && ++entry.frame_counter >= display_hz_ / hz) {
            entry.screen->animate();
            entry.frame_counter = 0;
        }
    }

    current_collisions_.clear();
    for (size_t i = 0; i < entries_.size(); ++i) {
        for (size_t j = i + 1; j < entries_.size(); ++j) {
            Screen* a = entries_[i].screen;
            Screen* b = entries_[j].screen;
            if (a->bounds().intersects(b->bounds()))
                current_collisions_.push_back({a, b});
        }
    }

    for (auto& pair : current_collisions_) {
        bool was_colliding = false;
        for (auto& active : active_collisions_)
            if (active == pair) { was_colliding = true; break; }
        if (!was_colliding) {
            pair.a->on_collision(*pair.b);
            pair.b->on_collision(*pair.a);
        }
    }

    in_animate_ = false;

    for (auto& op : pending_) {
        if (op.kind == PendingOp::Kind::Add)
            entries_.push_back({op.screen, 0});
        else
            do_remove(*op.screen);
    }
    pending_.clear();

    std::swap(active_collisions_, current_collisions_);
    critical_section_exit(&cs_);
}

void Scene::adjust_speed(int delta) {
    critical_section_enter_blocking(&cs_);
    for (auto& entry : entries_)
        entry.screen->adjust_speed(delta);
    critical_section_exit(&cs_);
}

void Scene::reset() {
    critical_section_enter_blocking(&cs_);
    for (auto& entry : entries_) {
        entry.screen->reset();
        entry.frame_counter = 0;
    }
    active_collisions_.clear();
    critical_section_exit(&cs_);
}
