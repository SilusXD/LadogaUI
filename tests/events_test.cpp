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

std::unique_ptr<ladoga::Button> make_button(int width, int height) {
    auto button = std::make_unique<ladoga::Button>();
    button->set_width(width);
    button->set_height(height);
    return button;
}

} // namespace

int main() {
    {
        ladoga::Window window("Events", 400, 300);
        auto button = make_button(100, 50);
        int clicks = 0;
        button->clicked = [&clicks] { ++clicks; };
        ladoga::Button* button_ptr = button.get();
        window.add_child(std::move(button));
        window.set_bounds({0, 0, 400, 300});
        window.layout();

        check(button_ptr->bounds() == ladoga::Rect{0, 0, 100, 50}, "button laid out");
        window.dispatch_event(mouse(ladoga::EventType::MouseDown, 20, 20));
        window.dispatch_event(mouse(ladoga::EventType::MouseUp, 20, 20));
        check(clicks == 1, "click inside fires once");
    }

    {
        ladoga::Window window("Events", 400, 300);
        auto button = make_button(100, 50);
        int clicks = 0;
        button->clicked = [&clicks] { ++clicks; };
        window.add_child(std::move(button));
        window.set_bounds({0, 0, 400, 300});
        window.layout();

        window.dispatch_event(mouse(ladoga::EventType::MouseDown, 20, 20));
        window.dispatch_event(mouse(ladoga::EventType::MouseUp, 300, 200));
        check(clicks == 0, "release outside does not fire");
    }

    {
        ladoga::Window window("Events", 400, 300);
        auto first = make_button(100, 50);
        auto second = make_button(100, 50);
        int first_clicks = 0;
        int second_clicks = 0;
        first->clicked = [&first_clicks] { ++first_clicks; };
        second->clicked = [&second_clicks] { ++second_clicks; };
        window.add_child(std::move(first));
        window.add_child(std::move(second));
        window.set_bounds({0, 0, 400, 300});
        // Намеренно кладём кнопки друг на друга, минуя StackPanel-раскладку окна.
        window.children()[0]->set_bounds({0, 0, 100, 50});
        window.children()[1]->set_bounds({0, 0, 100, 50});

        window.dispatch_event(mouse(ladoga::EventType::MouseDown, 10, 10));
        window.dispatch_event(mouse(ladoga::EventType::MouseUp, 10, 10));
        check(first_clicks == 0, "bottom button not clicked");
        check(second_clicks == 1, "topmost button clicked");
    }

    if (failures == 0) {
        std::cout << "events test: OK\n";
        return 0;
    }
    std::cerr << "events test: " << failures << " failure(s)\n";
    return 1;
}
