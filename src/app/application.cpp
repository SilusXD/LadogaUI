#include <ladoga/application.h>

#include <ladoga/core/color.h>
#include <ladoga/window.h>

#include "render/opengl_renderer.h"
#include "render/window_backend.h"

namespace ladoga {

Application::Application(int, char**) {}

Application::~Application() = default;

int Application::run(Window& window) {
    SDL3Backend backend(window.title(), window.width(), window.height());
    OpenGLRenderer renderer;

    while (backend.poll_events()) {
        renderer.set_viewport(backend.drawable_size());
        renderer.begin_frame(Color{16, 20, 28, 255});
        window.paint(renderer);
        renderer.end_frame();
        backend.present();
    }

    return 0;
}

} // namespace ladoga
