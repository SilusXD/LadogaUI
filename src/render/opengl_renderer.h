#pragma once

#include <ladoga/renderer.h>

#include "render/font.h"

namespace ladoga {

/// OpenGL-рендерер: прямоугольники и текст через fixed-function OpenGL.
/// TODO: позже заменить на шейдеры и батчинг.
class OpenGLRenderer final : public Renderer {
public:
    OpenGLRenderer() = default;
    ~OpenGLRenderer() override = default;

    void set_viewport(Size size) override;
    void begin_frame(Color clear_color) override;
    void draw_rect(const Rect& rect, Color color) override;
    void draw_text(const Rect& rect, std::string_view text, Color color) override;
    void end_frame() override;

private:
    Size viewport_{1, 1};
    Font font_;
};

} // namespace ladoga
