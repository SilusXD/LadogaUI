#include <ladoga/window.h>

#include <algorithm>
#include <iostream>
#include <utility>

#include <ladoga/core/event.h>

#include "xml/xml_loader.h"

namespace ladoga {

namespace {

int fixed_or(int value, int preferred) { return value == Widget::Auto ? preferred : value; }

} // namespace

Window::Window(std::string title, int width, int height) : title_(std::move(title)) {
    set_width(width);
    set_height(height);
}

const std::string& Window::title() const noexcept { return title_; }

void Window::set_title(std::string title) { title_ = std::move(title); }

void Window::set_size(int width, int height) {
    if (width > 0) {
        set_width(width);
    }
    if (height > 0) {
        set_height(height);
    }
}

void Window::load_from_xml(const std::filesystem::path& path) {
    mouse_capture_ = nullptr;
    focused_widget_ = nullptr;
    clear_children();
    xml::XmlLoader loader;
    loader.load(*this, path);
}

bool Window::dispatch_event(const Event& event) {
    if (event.type == EventType::MouseDown) {
        Widget* target = hit_test(event.position);
        focus_on(target);
        mouse_capture_ = target;
        return target != nullptr && target->dispatch_event(event);
    }
    if (event.type == EventType::MouseUp) {
        Widget* target = mouse_capture_ != nullptr ? mouse_capture_ : hit_test(event.position);
        mouse_capture_ = nullptr;
        return target != nullptr && target->dispatch_event(event);
    }
    if (event.type == EventType::MouseMove && mouse_capture_ != nullptr) {
        return mouse_capture_->dispatch_event(event);
    }
    if (event.type == EventType::KeyDown || event.type == EventType::TextInput) {
        return focused_widget_ != nullptr && focused_widget_->dispatch_event(event);
    }
    return false;
}

void Window::focus_on(Widget* target) {
    Widget* next = target != nullptr && target->accepts_focus() ? target : nullptr;
    if (next == focused_widget_) {
        return;
    }
    if (focused_widget_ != nullptr) {
        focused_widget_->set_focus(false);
    }
    focused_widget_ = next;
    if (focused_widget_ != nullptr) {
        focused_widget_->set_focus(true);
    }
}

void Window::register_action(std::string name, std::function<void()> action) {
    actions_[std::move(name)] = std::move(action);
}

bool Window::invoke_action(std::string_view name) {
    const auto it = actions_.find(name);
    if (it == actions_.end()) {
        std::cerr << "LadogaUI: действие '" << name << "' не зарегистрировано\n";
        return false;
    }
    if (it->second) {
        it->second();
    }
    return true;
}

void Window::layout() {
    Rect area = bounds();
    if (area.width <= 0) {
        area.width = width() == Auto ? 800 : width();
    }
    if (area.height <= 0) {
        area.height = height() == Auto ? 600 : height();
    }
    set_bounds(area);
    layout_children();
}

void Window::layout_children() {
    const Rect area = bounds();
    const Margin window_margin = margin();
    const int left = area.x + window_margin.left;
    const int top = area.y + window_margin.top;
    int cursor = top;

    for (const auto& child : children()) {
        if (!child->visible()) {
            continue;
        }
        const Size desired = child->preferred_size();
        const Margin child_margin = child->margin();
        const int child_width = fixed_or(child->width(), desired.width);
        const int child_height = fixed_or(child->height(), desired.height);

        child->set_bounds({left + child_margin.left, cursor + child_margin.top, child_width,
                           child_height});
        child->layout();
        cursor += child_margin.top + child_height + child_margin.bottom;
    }
}

} // namespace ladoga
