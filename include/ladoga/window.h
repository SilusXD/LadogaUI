#pragma once

#include <string>

#include <ladoga/renderer.h>

namespace ladoga {

/// Корневое окно. На этапе 1 хранит заголовок и размер, а также умеет
/// рисовать себя. Наследование от Widget появится на этапе 2.
class Window {
public:
    explicit Window(std::string title = "LadogaUI", int width = 800, int height = 600);
    virtual ~Window() = default;

    const std::string& title() const noexcept;
    void set_title(std::string title);

    int width() const noexcept;
    int height() const noexcept;
    void set_size(int width, int height);

    virtual void paint(Renderer& renderer);

private:
    std::string title_;
    int width_ = 800;
    int height_ = 600;
};

} // namespace ladoga
