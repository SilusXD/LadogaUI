#include <algorithm>
#include <iostream>

#include <ladoga/ladoga.h>

#include "render/font.h"

namespace {

int failures = 0;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

} // namespace

int main() {
    ladoga::Font font;
    check(font.valid(), "font loaded");
    if (font.valid()) {
        const auto bitmap =
            font.render("Привет, мир! Hello", 16, ladoga::Color{255, 255, 255, 255});
        check(bitmap.width > 0, "text bitmap width");
        check(bitmap.height > 0, "text bitmap height");
        const bool has_pixels = std::any_of(bitmap.pixels.begin(), bitmap.pixels.end(),
                                            [](std::uint8_t value) { return value != 0; });
        check(has_pixels, "text bitmap has glyph pixels");
    }

    if (failures == 0) {
        std::cout << "font test: OK\n";
        return 0;
    }
    std::cerr << "font test: " << failures << " failure(s)\n";
    return 1;
}
