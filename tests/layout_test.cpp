#include <iostream>
#include <memory>

#include <ladoga/ladoga.h>

namespace {

int failures = 0;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

std::unique_ptr<ladoga::Widget> make_box(int width, int height, ladoga::Margin margin = {}) {
    auto box = std::make_unique<ladoga::Widget>();
    box->set_width(width);
    box->set_height(height);
    box->set_margin(margin);
    return box;
}

} // namespace

int main() {
    {
        ladoga::StackPanel panel;
        panel.set_width(300);
        panel.set_height(200);
        panel.set_bounds({10, 20, 0, 0});

        auto first = make_box(100, 40);
        auto second = make_box(120, 50, {10, 5, 10, 5});
        ladoga::Widget* first_ptr = first.get();
        ladoga::Widget* second_ptr = second.get();
        panel.add_child(std::move(first));
        panel.add_child(std::move(second));

        panel.layout();
        check(first_ptr->bounds() == ladoga::Rect{10, 20, 100, 40}, "vertical: first position");
        check(second_ptr->bounds() == ladoga::Rect{20, 65, 120, 50}, "vertical: margin position");
    }

    {
        ladoga::StackPanel panel;
        panel.set_orientation(ladoga::Orientation::Horizontal);
        panel.set_width(400);
        panel.set_height(100);
        panel.set_bounds({10, 20, 0, 0});

        auto first = make_box(100, 40);
        auto second = make_box(120, 50, {10, 5, 10, 5});
        ladoga::Widget* first_ptr = first.get();
        ladoga::Widget* second_ptr = second.get();
        panel.add_child(std::move(first));
        panel.add_child(std::move(second));

        panel.layout();
        check(first_ptr->bounds() == ladoga::Rect{10, 20, 100, 40}, "horizontal: first position");
        check(second_ptr->bounds() == ladoga::Rect{120, 25, 120, 50},
              "horizontal: margin position");
    }

    {
        ladoga::StackPanel panel;
        panel.add_child(make_box(100, 30));
        panel.add_child(make_box(80, 40));
        panel.set_bounds({0, 0, 0, 0});

        panel.layout();
        check(panel.bounds().width == 100, "auto: panel width");
        check(panel.bounds().height == 70, "auto: panel height");
        check(panel.children()[0]->bounds() == ladoga::Rect{0, 0, 100, 30},
              "auto: first child");
        check(panel.children()[1]->bounds() == ladoga::Rect{0, 30, 80, 40},
              "auto: second child");
    }

    {
        ladoga::Window window("Test", 800, 600);
        auto panel = std::make_unique<ladoga::StackPanel>();
        panel->set_name("root");
        panel->set_margin(16);
        auto* panel_ptr = panel.get();
        window.add_child(std::move(panel));

        check(window.find_child("root") == panel_ptr, "find_child by name");
        check(window.find_child<ladoga::StackPanel>("root") == panel_ptr, "find_child by type");
        check(window.find_child<ladoga::Widget>("missing") == nullptr, "missing child");
    }

    if (failures == 0) {
        std::cout << "layout test: OK\n";
        return 0;
    }
    std::cerr << "layout test: " << failures << " failure(s)\n";
    return 1;
}
