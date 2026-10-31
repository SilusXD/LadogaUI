#include "render/font.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace ladoga {

struct Font::Impl {
    std::vector<unsigned char> data;
    stbtt_fontinfo info{};
    bool valid = false;
};

namespace {

std::vector<int> decode_utf8(std::string_view text) {
    std::vector<int> result;
    for (std::size_t i = 0; i < text.size();) {
        const unsigned char first = static_cast<unsigned char>(text[i]);
        int codepoint = 0;
        int length = 0;

        if (first < 0x80) {
            codepoint = first;
            length = 1;
        } else if ((first & 0xE0) == 0xC0 && i + 1 < text.size()) {
            codepoint = ((first & 0x1F) << 6) |
                        (static_cast<unsigned char>(text[i + 1]) & 0x3F);
            length = 2;
        } else if ((first & 0xF0) == 0xE0 && i + 2 < text.size()) {
            codepoint = ((first & 0x0F) << 12) |
                        ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(text[i + 2]) & 0x3F);
            length = 3;
        } else if ((first & 0xF8) == 0xF0 && i + 3 < text.size()) {
            codepoint = ((first & 0x07) << 18) |
                        ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12) |
                        ((static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(text[i + 3]) & 0x3F);
            length = 4;
        } else {
            ++i;
            continue;
        }

        result.push_back(codepoint);
        i += static_cast<std::size_t>(length);
    }
    return result;
}

std::vector<std::filesystem::path> font_candidates() {
    std::vector<std::filesystem::path> paths;
#ifdef LADOGA_FONT_PATH
    paths.emplace_back(LADOGA_FONT_PATH);
#endif
    if (const char* base = SDL_GetBasePath()) {
        paths.emplace_back(std::filesystem::path(base) / "fonts" / "Roboto-Regular.ttf");
    }
    paths.emplace_back(std::filesystem::current_path() / "third_party" / "Roboto-Regular.ttf");

#if defined(_WIN32)
    paths.emplace_back("C:/Windows/Fonts/segoeui.ttf");
    paths.emplace_back("C:/Windows/Fonts/arial.ttf");
#elif defined(__APPLE__)
    paths.emplace_back("/System/Library/Fonts/Supplemental/Arial.ttf");
#else
    paths.emplace_back("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    paths.emplace_back("/usr/share/fonts/TTF/DejaVuSans.ttf");
#endif
    return paths;
}

bool load_file(const std::filesystem::path& path, std::vector<unsigned char>& data) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return !data.empty();
}

} // namespace

Font::Font() : impl_(std::make_unique<Impl>()) {
    for (const auto& path : font_candidates()) {
        if (!load_file(path, impl_->data)) {
            continue;
        }
        const int offset = stbtt_GetFontOffsetForIndex(impl_->data.data(), 0);
        if (offset >= 0 && stbtt_InitFont(&impl_->info, impl_->data.data(), offset)) {
            impl_->valid = true;
            return;
        }
        impl_->data.clear();
    }
    std::cerr << "LadogaUI: шрифт не найден, draw_text ничего не нарисует\n";
}

Font::~Font() = default;

bool Font::valid() const noexcept { return impl_->valid; }

Font::Bitmap Font::render(std::string_view text, int pixel_height, Color color) const {
    Bitmap bitmap;
    if (!impl_->valid || text.empty() || pixel_height <= 0) {
        return bitmap;
    }

    const auto codepoints = decode_utf8(text);
    if (codepoints.empty()) {
        return bitmap;
    }

    const float scale = stbtt_ScaleForPixelHeight(&impl_->info, static_cast<float>(pixel_height));
    int ascent = 0;
    int descent = 0;
    int line_gap = 0;
    stbtt_GetFontVMetrics(&impl_->info, &ascent, &descent, &line_gap);

    const int baseline = static_cast<int>(std::ceil(static_cast<float>(ascent) * scale));
    const int height = baseline + static_cast<int>(std::ceil(static_cast<float>(-descent) * scale)) + 2;

    std::vector<int> advances;
    advances.reserve(codepoints.size());
    double text_width = 0.0;
    for (std::size_t i = 0; i < codepoints.size(); ++i) {
        int advance = 0;
        int left_bearing = 0;
        stbtt_GetCodepointHMetrics(&impl_->info, codepoints[i], &advance, &left_bearing);
        advances.push_back(advance);
        text_width += static_cast<double>(advance) * scale;
        if (i + 1 < codepoints.size()) {
            text_width += static_cast<double>(
                              stbtt_GetCodepointKernAdvance(&impl_->info, codepoints[i],
                                                            codepoints[i + 1])) *
                          scale;
        }
    }

    bitmap.width = static_cast<int>(std::ceil(text_width)) + 2;
    bitmap.height = height + 2;
    if (bitmap.width <= 2 || bitmap.height <= 2) {
        return {};
    }
    bitmap.pixels.assign(static_cast<std::size_t>(bitmap.width) * bitmap.height * 4, 0);

    double pen_x = 1.0;
    for (std::size_t i = 0; i < codepoints.size(); ++i) {
        int width = 0;
        int glyph_height = 0;
        int x_offset = 0;
        int y_offset = 0;
        unsigned char* glyph = stbtt_GetCodepointBitmap(
            &impl_->info, 0.0F, scale, codepoints[i], &width, &glyph_height, &x_offset, &y_offset);
        if (glyph != nullptr) {
            const int base_x = static_cast<int>(pen_x) + x_offset;
            const int base_y = baseline + y_offset + 1;
            for (int y = 0; y < glyph_height; ++y) {
                const int target_y = base_y + y;
                if (target_y < 0 || target_y >= bitmap.height) {
                    continue;
                }
                for (int x = 0; x < width; ++x) {
                    const int target_x = base_x + x;
                    if (target_x < 0 || target_x >= bitmap.width) {
                        continue;
                    }
                    const std::uint8_t coverage = glyph[y * width + x];
                    if (coverage == 0) {
                        continue;
                    }
                    const std::size_t index =
                        (static_cast<std::size_t>(target_y) * bitmap.width + target_x) * 4;
                    bitmap.pixels[index + 0] = color.r;
                    bitmap.pixels[index + 1] = color.g;
                    bitmap.pixels[index + 2] = color.b;
                    bitmap.pixels[index + 3] =
                        static_cast<std::uint8_t>(coverage * color.a / 255);
                }
            }
            stbtt_FreeBitmap(glyph, nullptr);
        }

        pen_x += static_cast<double>(advances[i]) * scale;
        if (i + 1 < codepoints.size()) {
            pen_x += static_cast<double>(
                         stbtt_GetCodepointKernAdvance(&impl_->info, codepoints[i],
                                                       codepoints[i + 1])) *
                     scale;
        }
    }

    return bitmap;
}

} // namespace ladoga
