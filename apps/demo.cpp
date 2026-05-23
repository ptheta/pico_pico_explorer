#include "demo.hpp"
#include <cstdlib>

static uint8_t light_channel() { return static_cast<uint8_t>(150 + rand() % 106); }

Demo::Demo()
    : text_("Hello, World!"), scene_({0, 32, 96})
{
    scene_.add(box_);
    scene_.add(circle_);
    scene_.add(text_);
}

void Demo::render(Renderer& r) const { scene_.render(r); }
void Demo::animate()                 { scene_.animate(); }
uint32_t Demo::animation_hz() const  { return scene_.animation_hz(); }
void Demo::adjust_speed(int delta)   { scene_.adjust_speed(delta); }

void Demo::reset() {
    for (auto& t : triangles_) scene_.remove(*t);
    triangles_.clear();
    scene_.reset();
}

void Demo::add_triangle() {
    Colour c = {light_channel(), light_channel(), light_channel()};
    auto t = std::make_unique<TriangleScreen>(TriangleScreen::DEFAULT_SIZE, c);
    scene_.add(*t);
    triangles_.push_back(std::move(t));
}

void Demo::remove_triangle() {
    if (triangles_.empty()) return;
    int idx = rand() % static_cast<int>(triangles_.size());
    scene_.remove(*triangles_[idx]);
    triangles_.erase(triangles_.begin() + idx);
}
