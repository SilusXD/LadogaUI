#include <ladoga/button.h>

#include <algorithm>
#include <utility>

#include <ladoga/core/event.h>

#include "text_metrics.h"

namespace ladoga {

namespace {

constexpr int button_height = 32;

} // namespace

const std::string& Button::text() const noexcept { return text_; }

void Button::set_text(std::string value) { text_ = std::move(value); }

Size Button::preferred_size() const {
    const int text_width = approximate_text_width(text_) + 24;
    const int desired_width = std::max(text_width, 80);
    return {width() == Auto ? desired_width : width(),
            height() == Auto ? button_height : height()};
}

bool Button::on_event(const Event& event) {
    if (event.type == EventType::MouseDown && event.button == MouseButton::Left) {
        pressed_ = true;
        return true;
    }
    if (event.type == EventType::MouseUp && event.button == MouseButton::Left) {
        const bool fire = pressed_ && bounds().contains(event.position);
        pressed_ = false;
        if (fire && clicked) {
            clicked();
        }
        return true;
    }
    return false;
}

void Button::paint_self(Renderer& renderer) {
    const Color color = pressed_ ? Color{50, 105, 180, 255} : Color{74, 144, 226, 255};
    renderer.draw_rect(bounds(), color);

    const int text_width = approximate_text_width(text_);
    const int x = bounds().x + std::max(4, (bounds().width - text_width) / 2);
    renderer.draw_text({x, bounds().y, bounds().width - (x - bounds().x), bounds().height}, text_,
                       Color::white());
}

} // namespace ladoga
