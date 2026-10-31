#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

#include <ladoga/core/color.h>

namespace ladoga {

/// Растеризация текста через stb_truetype. Внутренний класс рендерера.
class Font {
public:
    struct Bitmap {
        int width = 0;
        int height = 0;
        std::vector<std::uint8_t> pixels;
    };

    Font();
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] Bitmap render(std::string_view text, int pixel_height, Color color) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace ladoga
