#include <iostream>

#include <ladoga/ladoga.h>

int main() {
    std::cout << "LadogaUI " << ladoga::version() << '\n';

    const ladoga::Rect frame{0, 0, 400, 300};
    std::cout << "Frame: " << frame.width << "x" << frame.height << '\n';
    std::cout << "Scaffold is ready. Next: SDL3 window and rectangle.\n";

    return 0;
}
