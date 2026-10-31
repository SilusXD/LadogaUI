#pragma once

#include <functional>
#include <string>

#include <ladoga/widget.h>

namespace ladoga {

/// Флаг вкл/выкл. Аналог QCheckBox / WPF CheckBox.
class CheckBox : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);

    bool checked() const noexcept;
    void set_checked(bool value);

    Size preferred_size() const override;
    bool on_event(const Event& event) override;

    std::function<void(bool)> checked_changed;

protected:
    void paint_self(Renderer& renderer) override;

private:
    std::string text_;
    bool checked_ = false;
};

} // namespace ladoga
