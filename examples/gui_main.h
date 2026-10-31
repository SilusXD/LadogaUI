#pragma once

#include <exception>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace ladoga::examples {

/// В GUI-приложении stderr не виден: показываем ошибку диалогом.
inline void report_error(const std::exception& error) {
#if defined(_WIN32)
    MessageBoxA(nullptr, error.what(), "LadogaUI", MB_ICONERROR | MB_OK);
#else
    std::cerr << "LadogaUI error: " << error.what() << '\n';
#endif
}

} // namespace ladoga::examples

#if defined(_WIN32)
// WIN32_EXECUTABLE ищет WinMain; вызываем из него обычный main-код.
int ladoga_example_main();
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return ladoga_example_main(); }
#define LADOGA_EXAMPLE_MAIN int ladoga_example_main()
#else
#define LADOGA_EXAMPLE_MAIN int main()
#endif
