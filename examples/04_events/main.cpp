#include <exception>
#include <iostream>
#include <string>

#include <ladoga/ladoga.h>

class EventsWindow final : public ladoga::Window {
public:
    EventsWindow() : Window("LadogaUI - Events", 560, 420) {
        auto root = std::make_unique<ladoga::StackPanel>();
        root->set_margin(24);

        auto label = std::make_unique<ladoga::Label>();
        label->set_name("status");
        label->set_text("Нажмите кнопку");
        status_ = label.get();
        root->add_child(std::move(label));

        auto button = std::make_unique<ladoga::Button>();
        button->set_name("button");
        button->set_text("Нажми меня");
        button->clicked = [this] { status_->set_text("Кнопка нажата"); };
        root->add_child(std::move(button));

        auto check = std::make_unique<ladoga::CheckBox>();
        check->set_name("check");
        check->set_text("Флажок");
        check->set_checked(true);
        check->checked_changed = [this](bool checked) {
            status_->set_text(checked ? "Флажок включён" : "Флажок выключен");
        };
        root->add_child(std::move(check));

        auto slider = std::make_unique<ladoga::Slider>();
        slider->set_name("slider");
        slider->set_range(0, 100);
        slider->set_value(50);
        slider->value_changed = [this](int value) {
            status_->set_text("Значение: " + std::to_string(value));
        };
        root->add_child(std::move(slider));

        add_child(std::move(root));
    }

private:
    ladoga::Label* status_ = nullptr;
};

int main(int argc, char** argv) {
    try {
        ladoga::Application app(argc, argv);
        EventsWindow window;
        return app.run(window);
    } catch (const std::exception& error) {
        std::cerr << "LadogaUI error: " << error.what() << '\n';
        return 1;
    }
}
