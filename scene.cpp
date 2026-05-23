#include "scene.hpp"

Scene::Scene(Colour bg, uint32_t display_hz)
    : bg_(bg), display_hz_(display_hz) {}

void Scene::add(Screen& screen) {
    entries_.push_back({&screen, 0});
}

void Scene::render(Renderer& r) const {
    r.set_pen(bg_);
    r.clear();
    for (const auto& entry : entries_) {
        entry.screen->render(r);
    }
}

void Scene::animate() {
    for (auto& entry : entries_) {
        uint32_t hz = entry.screen->animation_hz();
        if (hz > 0 && ++entry.frame_counter >= display_hz_ / hz) {
            entry.screen->animate();
            entry.frame_counter = 0;
        }
    }

    for (size_t i = 0; i < entries_.size(); ++i) {
        for (size_t j = i + 1; j < entries_.size(); ++j) {
            if (entries_[i].screen->bounds().intersects(entries_[j].screen->bounds())) {
                entries_[i].screen->on_collision(*entries_[j].screen);
                entries_[j].screen->on_collision(*entries_[i].screen);
            }
        }
    }
}
