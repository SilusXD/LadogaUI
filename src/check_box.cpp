#include <ladoga/check_box.h>

#include <algorithm>
#include <utility>

namespace ladoga {

namespace {

constexpr int approximate_char_width = 8;
constexpr int check_box_height = 24;

} // namespace

const std::string& CheckBox::text() const noexcept { return text_; }

void CheckBox::set_text(std::string value) { text_ = std::move(value); }

bool CheckBox::checked() const noexcept { return checked_; }

void CheckBox::set_checked(bool value) { checked_ = value; }

Size CheckBox::preferred_size() const {
    const int text_width = static_cast<int>(text_.size()) * approximate_char_width + 32;
    const int desired_width = std::max(text_width, 24);
    return {width() == Auto ? desired_width : width(),
            height() == Auto ? check_box_height : height()};
}

void CheckBox::paint_self(Renderer& renderer) {
    // TODO: упрощение для этапа 3: галочка и текст — этапы 4–5.
    const int box_size = std::min(16, bounds().height);
    renderer.draw_rect({bounds().x, bounds().y + (bounds().height - box_size) / 2, box_size,
                        box_size},
                       checked_ ? Color{80, 190, 120, 255} : Color{150, 155, 165, 255});
    if (bounds().width > 24) {
        renderer.draw_rect({bounds().x + 24, bounds().y, bounds().width - 24, bounds().height},
                           Color{70, 86, 112, 255});
    }
}

} // namespace ladoga
