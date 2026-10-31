#include <ladoga/stack_panel.h>

#include <algorithm>

namespace ladoga {

namespace {

int fixed_or(int value, int preferred) { return value == Widget::Auto ? preferred : value; }

} // namespace

Orientation StackPanel::orientation() const noexcept { return orientation_; }

void StackPanel::set_orientation(Orientation value) { orientation_ = value; }

Size StackPanel::preferred_size() const {
    int main = 0;
    int cross = 0;

    for (const auto& child : children()) {
        if (!child->visible()) {
            continue;
        }
        const Size desired = child->preferred_size();
        const Margin margin = child->margin();
        const int width = fixed_or(child->width(), desired.width);
        const int height = fixed_or(child->height(), desired.height);

        if (orientation_ == Orientation::Vertical) {
            main += margin.top + height + margin.bottom;
            cross = std::max(cross, margin.left + width + margin.right);
        } else {
            main += margin.left + width + margin.right;
            cross = std::max(cross, margin.top + height + margin.bottom);
        }
    }

    const int panel_width =
        width() == Auto ? (orientation_ == Orientation::Vertical ? cross : main) : width();
    const int panel_height =
        height() == Auto ? (orientation_ == Orientation::Vertical ? main : cross) : height();
    return {panel_width, panel_height};
}

void StackPanel::layout_children() {
    const Rect area = bounds();
    int cursor = orientation_ == Orientation::Vertical ? area.y : area.x;

    for (const auto& child : children()) {
        if (!child->visible()) {
            continue;
        }

        const Size desired = child->preferred_size();
        const Margin margin = child->margin();
        const int child_width = fixed_or(child->width(), desired.width);
        const int child_height = fixed_or(child->height(), desired.height);

        if (orientation_ == Orientation::Vertical) {
            child->set_bounds(
                {area.x + margin.left, cursor + margin.top, child_width, child_height});
            cursor += margin.top + child_height + margin.bottom;
        } else {
            child->set_bounds(
                {cursor + margin.left, area.y + margin.top, child_width, child_height});
            cursor += margin.left + child_width + margin.right;
        }
        child->layout();
    }
}

} // namespace ladoga
