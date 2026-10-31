#include <ladoga/window.h>

#include <utility>

namespace ladoga {

Window::Window(std::string title, int width, int height)
    : title_(std::move(title)), width_(width), height_(height) {}

const std::string& Window::title() const noexcept { return title_; }

void Window::set_title(std::string title) { title_ = std::move(title); }

int Window::width() const noexcept { return width_; }

int Window::height() const noexcept { return height_; }

void Window::set_size(int width, int height) {
    if (width > 0) {
        width_ = width;
    }
    if (height > 0) {
        height_ = height;
    }
}

void Window::paint(Renderer&) {}

} // namespace ladoga
