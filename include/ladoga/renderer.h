#pragma once

#include <cstddef>
#include <string_view>

#include <ladoga/core/color.h>
#include <ladoga/core/geometry.h>

namespace ladoga {

/// Абстрактный рендерер. Виджеты рисуют через него и не знают про OpenGL.
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual void set_viewport(Size size) = 0;
    virtual void begin_frame(Color clear_color) = 0;
    virtual void draw_rect(const Rect& rect, Color color) = 0;
    virtual void draw_text(const Rect& rect, std::string_view text, Color color) = 0;
    virtual int text_width(std::string_view text) const = 0;
    virtual void end_frame() = 0;
};

} // namespace ladoga
