#include <ladoga/text_box.h>

#include <algorithm>
#include <utility>

namespace ladoga {

namespace {

constexpr int approximate_char_width = 8;
constexpr int text_box_height = 28;

} // namespace

const std::string& TextBox::text() const noexcept { return text_; }

void TextBox::set_text(std::string value) { text_ = std::move(value); }

Size TextBox::preferred_size() const {
    const int text_width = static_cast<int>(text_.size()) * approximate_char_width + 16;
    const int desired_width = std::max(text_width, 160);
    return {width() == Auto ? desired_width : width(),
            height() == Auto ? text_box_height : height()};
}

void TextBox::paint_self(Renderer& renderer) {
    // TODO: упрощение для этапа 3: текст, курсор и ввод — этап 5.
    renderer.draw_rect(bounds(), Color{225, 228, 235, 255});
}

} // namespace ladoga
