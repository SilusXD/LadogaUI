#pragma once

#include <functional>

#include <ladoga/widget.h>

namespace ladoga {

/// Числовой диапазон. Аналог QSlider / WPF Slider.
class Slider : public Widget {
public:
    int value() const noexcept;
    int min() const noexcept;
    int max() const noexcept;

    void set_min(int value);
    void set_max(int value);
    void set_range(int min, int max);
    void set_value(int value);

    Size preferred_size() const override;

    std::function<void(int)> value_changed;

protected:
    void paint_self(Renderer& renderer) override;

private:
    int value_ = 0;
    int min_ = 0;
    int max_ = 100;
};

} // namespace ladoga
