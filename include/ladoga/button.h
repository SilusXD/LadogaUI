#pragma once

#include <functional>
#include <string>

#include <ladoga/widget.h>

namespace ladoga {

/// Кликабельная кнопка. Аналог QPushButton / WPF Button.
class Button : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);

    Size preferred_size() const override;

    std::function<void()> clicked;

protected:
    void paint_self(Renderer& renderer) override;

private:
    std::string text_;
};

} // namespace ladoga
