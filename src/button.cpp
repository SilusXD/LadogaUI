#include <ladoga/button.h>

#include <algorithm>
#include <utility>

namespace ladoga {

namespace {

constexpr int approximate_char_width = 8;
constexpr int button_height = 32;

} // namespace

const std::string& Button::text() const noexcept { return text_; }

void Button::set_text(std::string value) { text_ = std::move(value); }

Size Button::preferred_size() const {
    const int text_width = static_cast<int>(text_.size()) * approximate_char_width + 24;
    const int desired_width = std::max(text_width, 80);
    return {width() == Auto ? desired_width : width(),
            height() == Auto ? button_height : height()};
}

void Button::paint_self(Renderer& renderer) {
    // TODO: упрощение для этапа 3: текст и состояние нажатия — этапы 4–5.
    renderer.draw_rect(bounds(), Color{74, 144, 226, 255});
}

} // namespace ladoga
