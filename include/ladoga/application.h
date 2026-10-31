#pragma once

namespace ladoga {

class Window;

/// Точка входа приложения: создаёт окно, рендерер и запускает цикл событий.
class Application {
public:
    Application(int argc, char** argv);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    int run(Window& window);
};

} // namespace ladoga
