#include <ladoga/check_box.h>

#include <algorithm>
#include <utility>

#include <ladoga/core/event.h>

#include "text_metrics.h"

namespace ladoga {

namespace {

constexpr int check_box_height = 24;

} // namespace

const std::string& CheckBox::text() const noexcept { return text_; }

void CheckBox::set_text(std::string value) { text_ = std::move(value); }

bool CheckBox::checked() const noexcept { return checked_; }

void CheckBox::set_checked(bool value) { checked_ = value; }

Size CheckBox::preferred_size() const {
    const int text_width = approximate_text_width(text_) + 32;
    const int desired_width = std::max(text_width, 24);
    return {width() == Auto ? desired_width : width(),
            height() == Auto ? check_box_height : height()};
}

bool CheckBox::on_event(const Event& event) {
    if (event.type == EventType::MouseUp && event.button == MouseButton::Left &&
        bounds().contains(event.position)) {
        checked_ = !checked_;
        if (checked_changed) {
            checked_changed(checked_);
        }
        return true;
    }
    return false;
}

void CheckBox::paint_self(Renderer& renderer) {
    const int box_size = std::min(16, bounds().height);
    const Rect box{bounds().x, bounds().y + (bounds().height - box_size) / 2, box_size, box_size};
    renderer.draw_rect(box, Color{210, 214, 222, 255});
    if (checked_) {
        renderer.draw_rect({box.x + 3, box.y + 3, box.width - 6, box.height - 6},
                           Color{80, 190, 120, 255});
    }
    if (bounds().width > 24) {
        renderer.draw_text({bounds().x + 24, bounds().y, bounds().width - 24, bounds().height},
                           text_, Color{235, 238, 245, 255});
    }
}

} // namespace ladoga
