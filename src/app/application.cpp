#include <ladoga/application.h>

#include <vector>

#include <ladoga/core/color.h>
#include <ladoga/core/event.h>
#include <ladoga/window.h>

#include "render/opengl_renderer.h"
#include "render/window_backend.h"

namespace ladoga {

Application::Application(int, char**) {}

Application::~Application() = default;

int Application::run(Window& window) {
    const int native_width = window.width() == Widget::Auto ? 800 : window.width();
    const int native_height = window.height() == Widget::Auto ? 600 : window.height();
    SDL3Backend backend(window.title(), native_width, native_height);
    OpenGLRenderer renderer;

    while (true) {
        const Size size = backend.drawable_size();
        window.set_bounds({0, 0, size.width, size.height});
        window.layout();

        std::vector<Event> events;
        const bool open = backend.poll_events(events);
        for (const Event& event : events) {
            window.dispatch_event(event);
        }
        backend.set_title(window.title());

        renderer.set_viewport(size);
        renderer.begin_frame(Color{16, 20, 28, 255});
        window.paint(renderer);
        renderer.end_frame();
        backend.present();

        if (!open) {
            break;
        }
    }

    return 0;
}

} // namespace ladoga
