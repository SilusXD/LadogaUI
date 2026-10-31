#include <exception>
#include <iostream>

#include <ladoga/ladoga.h>

#include "../gui_main.h"

class DemoWindow final : public ladoga::Window {
public:
    DemoWindow() : Window("LadogaUI - Stage 1", 800, 600) {}

    void paint(ladoga::Renderer& renderer) override {
        renderer.draw_rect({200, 150, 400, 300}, {74, 144, 226, 255});
    }
};

LADOGA_EXAMPLE_MAIN {
    try {
        ladoga::Application app(0, nullptr);
        DemoWindow window;
        return app.run(window);
    } catch (const std::exception& error) {
        ladoga::examples::report_error(error);
        return 1;
    }
}
