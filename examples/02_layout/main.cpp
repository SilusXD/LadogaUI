#include <exception>
#include <iostream>

#include <ladoga/ladoga.h>

#include "../gui_main.h"

class ColoredBox final : public ladoga::Widget {
public:
    explicit ColoredBox(ladoga::Color color) : color_(color) {}

    void paint(ladoga::Renderer& renderer) override {
        renderer.draw_rect(bounds(), color_);
        ladoga::Widget::paint(renderer);
    }

private:
    ladoga::Color color_;
};

class LayoutWindow final : public ladoga::Window {
public:
    LayoutWindow() : Window("LadogaUI - Stage 2", 800, 600) {
        auto root = std::make_unique<ladoga::StackPanel>();
        root->set_margin(40);

        auto red = std::make_unique<ColoredBox>(ladoga::Color{220, 80, 80, 255});
        red->set_width(400);
        red->set_height(100);
        root->add_child(std::move(red));

        auto nested = std::make_unique<ladoga::StackPanel>();
        nested->set_margin(20);

        auto blue = std::make_unique<ColoredBox>(ladoga::Color{74, 144, 226, 255});
        blue->set_width(300);
        blue->set_height(100);
        nested->add_child(std::move(blue));

        auto green = std::make_unique<ColoredBox>(ladoga::Color{80, 190, 120, 255});
        green->set_width(200);
        green->set_height(80);
        nested->add_child(std::move(green));

        root->add_child(std::move(nested));
        add_child(std::move(root));
    }
};

LADOGA_EXAMPLE_MAIN {
    try {
        ladoga::Application app(0, nullptr);
        LayoutWindow window;
        return app.run(window);
    } catch (const std::exception& error) {
        ladoga::examples::report_error(error);
        return 1;
    }
}
