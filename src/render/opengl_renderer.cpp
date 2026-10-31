#include "render/opengl_renderer.h"

#include <algorithm>

#include <SDL3/SDL_opengl.h>

namespace ladoga {

namespace {

float component(std::uint8_t value) { return static_cast<float>(value) / 255.0F; }

void set_color(Color color) {
    glColor4f(component(color.r), component(color.g), component(color.b), component(color.a));
}

} // namespace

void OpenGLRenderer::set_viewport(Size size) {
    viewport_ = {std::max(size.width, 1), std::max(size.height, 1)};
}

void OpenGLRenderer::begin_frame(Color clear_color) {
    glViewport(0, 0, viewport_.width, viewport_.height);
    glClearColor(component(clear_color.r), component(clear_color.g), component(clear_color.b),
                 component(clear_color.a));
    glClear(GL_COLOR_BUFFER_BIT);

    // Координаты виджетов: (0,0) слева сверху, Y растёт вниз.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, static_cast<double>(viewport_.width), static_cast<double>(viewport_.height), 0.0,
            -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
}

void OpenGLRenderer::draw_rect(const Rect& rect, Color color) {
    if (rect.empty()) {
        return;
    }

    set_color(color);
    glBegin(GL_QUADS);
    glVertex2f(static_cast<float>(rect.left()), static_cast<float>(rect.top()));
    glVertex2f(static_cast<float>(rect.right()), static_cast<float>(rect.top()));
    glVertex2f(static_cast<float>(rect.right()), static_cast<float>(rect.bottom()));
    glVertex2f(static_cast<float>(rect.left()), static_cast<float>(rect.bottom()));
    glEnd();
}

void OpenGLRenderer::draw_text(const Rect&, std::string_view, Color) {
    // TODO: текст появится на этапе 5.
}

void OpenGLRenderer::end_frame() {}

} // namespace ladoga
