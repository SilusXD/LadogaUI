#pragma once

#include <cstdint>

namespace ladoga {

/// Цвет в формате RGBA. Компоненты — 0..255.
struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    [[nodiscard]] static constexpr Color black() noexcept { return {0, 0, 0, 255}; }
    [[nodiscard]] static constexpr Color white() noexcept { return {255, 255, 255, 255}; }
    [[nodiscard]] static constexpr Color transparent() noexcept { return {0, 0, 0, 0}; }

    friend constexpr bool operator==(Color, Color) = default;
};

} // namespace ladoga
