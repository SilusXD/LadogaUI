#include <exception>
#include <iostream>

#include <ladoga/ladoga.h>

class DemoWindow final : public ladoga::Window {
public:
    DemoWindow() : Window("LadogaUI - Stage 1", 800, 600) {}

    void paint(ladoga::Renderer& renderer) override {
        renderer.draw_rect({200, 150, 400, 300}, {74, 144, 226, 255});
    }
};

int main(int argc, char** argv) {
    try {
        ladoga::Application app(argc, argv);
        DemoWindow window;
        return app.run(window);
    } catch (const std::exception& error) {
        std::cerr << "LadogaUI error: " << error.what() << '\n';
        return 1;
    }
}
