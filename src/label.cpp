#include <ladoga/label.h>

#include <algorithm>
#include <utility>

namespace ladoga {

namespace {

constexpr int approximate_char_width = 8;
constexpr int label_height = 24;

} // namespace

const std::string& Label::text() const noexcept { return text_; }

void Label::set_text(std::string value) { text_ = std::move(value); }

Size Label::preferred_size() const {
    const int text_width = static_cast<int>(text_.size()) * approximate_char_width + 8;
    const int desired_width = std::max(text_width, 24);
    return {width() == Auto ? desired_width : width(), height() == Auto ? label_height : height()};
}

void Label::paint_self(Renderer& renderer) {
    // TODO: упрощение для этапа 3: вместо текста рисуем заглушку. Текст — этап 5.
    renderer.draw_rect(bounds(), Color{70, 86, 112, 255});
}

} // namespace ladoga
