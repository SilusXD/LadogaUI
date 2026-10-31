#pragma once

namespace ladoga {

/// Внешние отступы виджета. Как QMargins / WPF Thickness.
struct Margin {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    [[nodiscard]] static constexpr Margin all(int value) noexcept {
        return {value, value, value, value};
    }

    friend constexpr bool operator==(Margin, Margin) = default;
};

} // namespace ladoga
