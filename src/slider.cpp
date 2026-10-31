#include <ladoga/slider.h>

#include <algorithm>
#include <stdexcept>
#include <string>

#include <ladoga/core/event.h>

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

bool Slider::on_event(const Event& event) {
    if (event.type == EventType::MouseDown && event.button == MouseButton::Left) {
        dragging_ = true;
        update_from_position(event.position.x);
        return true;
    }
    if (event.type == EventType::MouseMove && dragging_) {
        update_from_position(event.position.x);
        return true;
    }
    if (event.type == EventType::MouseUp && event.button == MouseButton::Left) {
        if (dragging_) {
            update_from_position(event.position.x);
            dragging_ = false;
        }
        return true;
    }
    return false;
}

void Slider::paint_self(Renderer& renderer) {
    const int track_y = bounds().y + bounds().height / 2 - 2;
    renderer.draw_rect({bounds().x, track_y, bounds().width, 4}, Color{120, 128, 140, 255});

    const int span = max_ - min_;
    const float ratio = span == 0 ? 0.0F : static_cast<float>(value_ - min_) / span;
    const int handle_width = 12;
    const int handle_x = bounds().x + static_cast<int>(ratio * (bounds().width - handle_width));
    renderer.draw_rect({handle_x, bounds().y + 4, handle_width, bounds().height - 8},
                       Color{74, 144, 226, 255});
    renderer.draw_text({bounds().x + 4, bounds().y, bounds().width - 8, bounds().height},
                       std::to_string(value_), Color{235, 238, 245, 255});
}

void Slider::update_from_position(int x) {
    if (bounds().width <= 0 || max_ <= min_) {
        return;
    }
    const float ratio =
        std::clamp(static_cast<float>(x - bounds().x) / bounds().width, 0.0F, 1.0F);
    const int next = min_ + static_cast<int>(ratio * static_cast<float>(max_ - min_) + 0.5F);
    if (next != value_) {
        value_ = next;
        if (value_changed) {
            value_changed(value_);
        }
    }
}

} // namespace ladoga
