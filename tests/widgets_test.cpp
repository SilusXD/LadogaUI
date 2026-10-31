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

ladoga::Event mouse(ladoga::EventType type, int x, int y) {
    ladoga::Event event;
    event.type = type;
    event.position = {x, y};
    event.button = ladoga::MouseButton::Left;
    return event;
}

} // namespace

int main() {
    {
        ladoga::Window window("Widgets", 500, 400);
        auto text_box = std::make_unique<ladoga::TextBox>();
        text_box->set_width(200);
        text_box->set_height(28);
        ladoga::TextBox* text_box_ptr = text_box.get();
        window.add_child(std::move(text_box));
        window.set_bounds({0, 0, 500, 400});
        text_box_ptr->set_bounds({0, 0, 200, 28});

        window.dispatch_event(mouse(ladoga::EventType::MouseDown, 10, 10));
        window.dispatch_event(mouse(ladoga::EventType::MouseUp, 10, 10));

        ladoga::Event input;
        input.type = ladoga::EventType::TextInput;
        input.text = "Привет";
        window.dispatch_event(input);
        input.text = "!";
        window.dispatch_event(input);
        check(text_box_ptr->text() == "Привет!", "TextBox accepts UTF-8 input");
    }

    {
        ladoga::Window window("Widgets", 500, 400);
        auto check_box = std::make_unique<ladoga::CheckBox>();
        check_box->set_width(120);
        check_box->set_height(24);
        check_box->set_checked(false);
        int changes = 0;
        bool last = false;
        check_box->checked_changed = [&changes, &last](bool value) {
            ++changes;
            last = value;
        };
        ladoga::CheckBox* check_box_ptr = check_box.get();
        window.add_child(std::move(check_box));
        window.set_bounds({0, 0, 500, 400});
        check_box_ptr->set_bounds({0, 0, 120, 24});

        window.dispatch_event(mouse(ladoga::EventType::MouseDown, 10, 10));
        window.dispatch_event(mouse(ladoga::EventType::MouseUp, 10, 10));
        check(changes == 1 && last, "CheckBox click fires checked_changed");
        check(check_box_ptr->checked(), "CheckBox toggled");
    }

    {
        ladoga::Window window("Widgets", 500, 400);
        auto slider = std::make_unique<ladoga::Slider>();
        slider->set_width(200);
        slider->set_height(28);
        slider->set_range(0, 100);
        int changes = 0;
        int last = 0;
        slider->value_changed = [&changes, &last](int value) {
            ++changes;
            last = value;
        };
        ladoga::Slider* slider_ptr = slider.get();
        window.add_child(std::move(slider));
        window.set_bounds({0, 0, 500, 400});
        slider_ptr->set_bounds({0, 0, 200, 28});

        window.dispatch_event(mouse(ladoga::EventType::MouseDown, 190, 14));
        check(slider_ptr->value() >= 90, "Slider click near right");
        window.dispatch_event(mouse(ladoga::EventType::MouseMove, 5, 14));
        check(slider_ptr->value() <= 5, "Slider drag near left");
        check(changes >= 1 && last <= 5, "Slider value_changed");
    }

    if (failures == 0) {
        std::cout << "widgets test: OK\n";
        return 0;
    }
    std::cerr << "widgets test: " << failures << " failure(s)\n";
    return 1;
}
