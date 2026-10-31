#include <iostream>

#include <ladoga/ladoga.h>

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
    check(!ladoga::version().empty(), "version is not empty");

    const ladoga::Rect rect{10, 20, 100, 50};
    check(rect.left() == 10, "rect.left()");
    check(rect.top() == 20, "rect.top()");
    check(rect.right() == 110, "rect.right()");
    check(rect.bottom() == 70, "rect.bottom()");
    check(rect.contains({10, 20}), "rect contains top-left corner");
    check(!rect.contains({110, 70}), "rect excludes bottom-right edge");
    check(!rect.contains({9, 20}), "rect excludes point to the left");

    const ladoga::Color white = ladoga::Color::white();
    check(white == ladoga::Color{255, 255, 255, 255}, "white color");

    if (failures == 0) {
        std::cout << "smoke test: OK\n";
        return 0;
    }

    std::cerr << "smoke test: " << failures << " failure(s)\n";
    return 1;
}
