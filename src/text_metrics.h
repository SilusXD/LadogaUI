#pragma once

#include <string_view>

namespace ladoga {

/// Грубая оценка ширины текста без шрифта: считаем кодовые точки, не байты.
/// Нужна только для preferred_size() до точной раскладки по шрифту.
inline int approximate_text_width(std::string_view text) {
    int codepoints = 0;
    for (unsigned char byte : text) {
        if ((byte & 0xC0) != 0x80) {
            ++codepoints;
        }
    }
    return codepoints * 8;
}

} // namespace ladoga
