#include <ladoga/label.h>

#include <algorithm>
#include <utility>

#include "text_metrics.h"

namespace ladoga {

namespace {

constexpr int label_height = 24;

} // namespace

const std::string& Label::text() const noexcept { return text_; }

void Label::set_text(std::string value) { text_ = std::move(value); }

Size Label::preferred_size() const {
    const int text_width = approximate_text_width(text_) + 8;
    const int desired_width = std::max(text_width, 24);
    return {width() == Auto ? desired_width : width(), height() == Auto ? label_height : height()};
}

void Label::paint_self(Renderer& renderer) {
    renderer.draw_text(bounds(), text_, Color{235, 238, 245, 255});
}

} // namespace ladoga
