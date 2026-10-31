#pragma once

#include <functional>
#include <string>

#include <ladoga/widget.h>

namespace ladoga {

/// Однострочный ввод. Аналог QLineEdit / WPF TextBox.
class TextBox : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);

    Size preferred_size() const override;

    bool on_event(const Event& event) override;
    [[nodiscard]] bool accepts_focus() const override;
    void set_focus(bool focused) override;

    std::function<void(const std::string&)> text_changed;

protected:
    void paint_self(Renderer& renderer) override;

private:
    void erase_last_codepoint();

    std::string text_;
    bool focused_ = false;
};

} // namespace ladoga
