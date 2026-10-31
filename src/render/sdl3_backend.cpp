#include "render/window_backend.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include <SDL3/SDL_opengl.h>

namespace ladoga {

namespace {

[[noreturn]] void fail(const char* what) {
    throw std::runtime_error(std::string(what) + ": " + SDL_GetError());
}

} // namespace

SDL3Backend::SDL3Backend(const std::string& title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fail("SDL_Init");
    }

    if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2) ||
        !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1) ||
        !SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) ||
        !SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
        cleanup();
        fail("SDL_GL_SetAttribute");
    }

    window_ = SDL_CreateWindow(title.c_str(), width, height,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr) {
        cleanup();
        fail("SDL_CreateWindow");
    }

    context_ = SDL_GL_CreateContext(window_);
    if (context_ == nullptr) {
        cleanup();
        fail("SDL_GL_CreateContext");
    }

    // Вертикальная синхронизация: не жжём CPU вхолостую. Если драйвер не умеет,
    // цикл всё равно работает.
    SDL_GL_SetSwapInterval(1);
}

SDL3Backend::~SDL3Backend() { cleanup(); }

bool SDL3Backend::poll_events() {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
            event.window.windowID == SDL_GetWindowID(window_)) {
            return false;
        }
    }
    return true;
}

Size SDL3Backend::drawable_size() const {
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window_, &width, &height);
    return {std::max(width, 1), std::max(height, 1)};
}

void SDL3Backend::present() { SDL_GL_SwapWindow(window_); }

void SDL3Backend::cleanup() noexcept {
    if (context_ != nullptr) {
        SDL_GL_DestroyContext(context_);
        context_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

} // namespace ladoga
