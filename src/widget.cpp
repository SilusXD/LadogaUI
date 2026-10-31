#include <ladoga/widget.h>

#include <algorithm>
#include <utility>

#include <ladoga/core/event.h>

namespace ladoga {

const std::string& Widget::name() const noexcept { return name_; }

void Widget::set_name(std::string value) { name_ = std::move(value); }

int Widget::width() const noexcept { return width_; }

int Widget::height() const noexcept { return height_; }

void Widget::set_width(int value) {
    if (value >= 0 || value == Auto) {
        width_ = value;
    }
}

void Widget::set_height(int value) {
    if (value >= 0 || value == Auto) {
        height_ = value;
    }
}

Margin Widget::margin() const noexcept { return margin_; }

void Widget::set_margin(Margin value) {
    margin_ = {std::max(value.left, 0), std::max(value.top, 0), std::max(value.right, 0),
               std::max(value.bottom, 0)};
}

void Widget::set_margin(int value) { set_margin(Margin::all(std::max(value, 0))); }

bool Widget::visible() const noexcept { return visible_; }

void Widget::set_visible(bool value) { visible_ = value; }

Rect Widget::bounds() const noexcept { return bounds_; }

void Widget::set_bounds(const Rect& value) { bounds_ = value; }

Widget* Widget::parent() const noexcept { return parent_; }

const std::vector<std::unique_ptr<Widget>>& Widget::children() const noexcept { return children_; }

void Widget::add_child(std::unique_ptr<Widget> child) {
    if (!child) {
        return;
    }
    child->parent_ = this;
    children_.push_back(std::move(child));
}

Widget* Widget::find_child(std::string_view name) noexcept {
    if (name_ == name) {
        return this;
    }
    for (const auto& child : children_) {
        if (Widget* found = child->find_child(name)) {
            return found;
        }
    }
    return nullptr;
}

const Widget* Widget::find_child(std::string_view name) const noexcept {
    if (name_ == name) {
        return this;
    }
    for (const auto& child : children_) {
        if (const Widget* found = child->find_child(name)) {
            return found;
        }
    }
    return nullptr;
}

Size Widget::preferred_size() const {
    return {width_ == Auto ? 0 : width_, height_ == Auto ? 0 : height_};
}

void Widget::layout() {
    Size desired = preferred_size();
    Rect area = bounds_;

    if (width_ != Auto) {
        area.width = width_;
    } else if (area.width <= 0) {
        area.width = desired.width;
    }

    if (height_ != Auto) {
        area.height = height_;
    } else if (area.height <= 0) {
        area.height = desired.height;
    }

    bounds_ = area;
    layout_children();
}

void Widget::paint(Renderer& renderer) {
    if (!visible_) {
        return;
    }
    paint_self(renderer);
    for (const auto& child : children_) {
        child->paint(renderer);
    }
}

bool Widget::on_event(const Event&) { return false; }

bool Widget::dispatch_event(const Event& event) {
    if (!visible_) {
        return false;
    }
    if (on_event(event)) {
        return true;
    }
    // Всплытие: предкам достаётся on_event, но не повторный hit-test.
    for (Widget* ancestor = parent_; ancestor != nullptr; ancestor = ancestor->parent_) {
        if (ancestor->on_event(event)) {
            return true;
        }
    }
    return false;
}

Widget* Widget::hit_test(Point point) {
    if (!visible_ || !bounds_.contains(point)) {
        return nullptr;
    }
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if (Widget* hit = (*it)->hit_test(point)) {
            return hit;
        }
    }
    return this;
}

bool Widget::accepts_focus() const { return false; }

void Widget::set_focus(bool) {}

void Widget::layout_children() {}

void Widget::paint_self(Renderer&) {}

void Widget::clear_children() noexcept { children_.clear(); }

} // namespace ladoga
