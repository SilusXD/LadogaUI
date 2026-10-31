#include "render/opengl_renderer.h"

#include <algorithm>

#include <SDL3/SDL_opengl.h>

namespace ladoga {

namespace {

constexpr int text_pixel_height = 16;

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

void OpenGLRenderer::draw_text(const Rect& rect, std::string_view text, Color color) {
    const Font::Bitmap bitmap = font_.render(text, text_pixel_height, color);
    if (bitmap.width <= 0 || bitmap.height <= 0) {
        return;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, bitmap.pixels.data());

    const int x = rect.x + 4;
    const int y = rect.y + (rect.height - bitmap.height) / 2;
    const float x2 = static_cast<float>(x + bitmap.width);
    const float y2 = static_cast<float>(y + bitmap.height);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 0.0F);
    glVertex2f(static_cast<float>(x), static_cast<float>(y));
    glTexCoord2f(1.0F, 0.0F);
    glVertex2f(x2, static_cast<float>(y));
    glTexCoord2f(1.0F, 1.0F);
    glVertex2f(x2, y2);
    glTexCoord2f(0.0F, 1.0F);
    glVertex2f(static_cast<float>(x), y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texture);
}

void OpenGLRenderer::end_frame() {}

} // namespace ladoga
