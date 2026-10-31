#pragma once

#include <string>

#include <ladoga/widget.h>

namespace ladoga {

/// Текст только для чтения. Аналог QLabel / WPF Label.
class Label : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);

    Size preferred_size() const override;

protected:
    void paint_self(Renderer& renderer) override;

private:
    std::string text_;
};

} // namespace ladoga
