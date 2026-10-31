#include <ladoga/text_box.h>

#include <algorithm>
#include <utility>

#include <SDL3/SDL_keycode.h>

#include <ladoga/core/event.h>

#include "text_metrics.h"

namespace ladoga {

namespace {

constexpr int text_box_height = 28;

} // namespace

const std::string& TextBox::text() const noexcept { return text_; }

void TextBox::set_text(std::string value) { text_ = std::move(value); }

Size TextBox::preferred_size() const {
    const int text_width = approximate_text_width(text_) + 16;
    const int desired_width = std::max(text_width, 160);
    return {width() == Auto ? desired_width : width(),
            height() == Auto ? text_box_height : height()};
}

bool TextBox::on_event(const Event& event) {
    if (event.type == EventType::MouseDown && event.button == MouseButton::Left) {
        return true;
    }
    if (event.type == EventType::KeyDown) {
        if (event.key == SDLK_BACKSPACE) {
            const std::string before = text_;
            erase_last_codepoint();
            if (text_ != before && text_changed) {
                text_changed(text_);
            }
            return true;
        }
        if (event.key == SDLK_RETURN || event.key == SDLK_KP_ENTER) {
            return true;
        }
    }
    if (event.type == EventType::TextInput) {
        if (!event.text.empty()) {
            text_ += event.text;
            if (text_changed) {
                text_changed(text_);
            }
        }
        return true;
    }
    return false;
}

bool TextBox::accepts_focus() const { return true; }

void TextBox::set_focus(bool focused) { focused_ = focused; }

void TextBox::paint_self(Renderer& renderer) {
    // TODO: упрощение для этапа 5: выделение текста и позиционирование курсора мышью — позже.
    renderer.draw_rect(bounds(), Color{232, 235, 242, 255});
    if (focused_) {
        const Color border{74, 144, 226, 255};
        renderer.draw_rect({bounds().x, bounds().y, bounds().width, 2}, border);
        renderer.draw_rect({bounds().x, bounds().bottom() - 2, bounds().width, 2}, border);
        renderer.draw_rect({bounds().x, bounds().y, 2, bounds().height}, border);
        renderer.draw_rect({bounds().right() - 2, bounds().y, 2, bounds().height}, border);
    }
    renderer.draw_text({bounds().x + 6, bounds().y, bounds().width - 12, bounds().height}, text_,
                       Color{24, 28, 36, 255});
    if (focused_) {
        const int cursor_x = bounds().x + 6 + approximate_text_width(text_);
        renderer.draw_rect({cursor_x, bounds().y + 6, 2, bounds().height - 12},
                           Color{30, 70, 140, 255});
    }
}

void TextBox::erase_last_codepoint() {
    if (text_.empty()) {
        return;
    }
    std::size_t index = text_.size() - 1;
    while (index > 0 && (static_cast<unsigned char>(text_[index]) & 0xC0) == 0x80) {
        --index;
    }
    text_.erase(index);
}

} // namespace ladoga
