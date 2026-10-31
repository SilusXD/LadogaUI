#include <ladoga/slider.h>

#include <algorithm>
#include <stdexcept>

namespace ladoga {

int Slider::value() const noexcept { return value_; }

int Slider::min() const noexcept { return min_; }

int Slider::max() const noexcept { return max_; }

void Slider::set_min(int value) {
    if (value > max_) {
        throw std::invalid_argument("Slider: Min не может быть больше Max");
    }
    min_ = value;
    value_ = std::clamp(value_, min_, max_);
}

void Slider::set_max(int value) {
    if (value < min_) {
        throw std::invalid_argument("Slider: Max не может быть меньше Min");
    }
    max_ = value;
    value_ = std::clamp(value_, min_, max_);
}

void Slider::set_range(int min, int max) {
    if (min > max) {
        throw std::invalid_argument("Slider: Min не может быть больше Max");
    }
    min_ = min;
    max_ = max;
    value_ = std::clamp(value_, min_, max_);
}

void Slider::set_value(int value) { value_ = std::clamp(value, min_, max_); }

Size Slider::preferred_size() const {
    return {width() == Auto ? 180 : width(), height() == Auto ? 28 : height()};
}

void Slider::paint_self(Renderer& renderer) {
    // TODO: упрощение для этапа 3: дорожка и бегунок без перетаскивания.
    const int track_y = bounds().y + bounds().height / 2 - 2;
    renderer.draw_rect({bounds().x, track_y, bounds().width, 4}, Color{120, 128, 140, 255});

    const int span = max_ - min_;
    const float ratio = span == 0 ? 0.0F : static_cast<float>(value_ - min_) / span;
    const int handle_width = 12;
    const int handle_x = bounds().x + static_cast<int>(ratio * (bounds().width - handle_width));
    renderer.draw_rect({handle_x, bounds().y + 4, handle_width, bounds().height - 8},
                       Color{74, 144, 226, 255});
}

} // namespace ladoga
