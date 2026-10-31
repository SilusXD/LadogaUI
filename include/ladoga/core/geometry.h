#pragma once

namespace ladoga {

/// Точка в координатах окна. Начало координат — левый верхний угол.
struct Point {
    int x = 0;
    int y = 0;

    friend constexpr bool operator==(Point, Point) = default;
};

/// Размер в пикселях.
struct Size {
    int width = 0;
    int height = 0;

    friend constexpr bool operator==(Size, Size) = default;
};

/// Прямоугольник в координатах окна.
struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    [[nodiscard]] constexpr int left() const noexcept { return x; }
    [[nodiscard]] constexpr int top() const noexcept { return y; }
    [[nodiscard]] constexpr int right() const noexcept { return x + width; }
    [[nodiscard]] constexpr int bottom() const noexcept { return y + height; }

    [[nodiscard]] constexpr bool empty() const noexcept { return width <= 0 || height <= 0; }

    /// Проверка попадания точки. Правый и нижний края не включаются,
    /// чтобы соседние прямоугольники не перекрывались в hit-test.
    [[nodiscard]] constexpr bool contains(Point point) const noexcept {
        return point.x >= x && point.x < right() && point.y >= y && point.y < bottom();
    }

    friend constexpr bool operator==(Rect, Rect) = default;
};

} // namespace ladoga
