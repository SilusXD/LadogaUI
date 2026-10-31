#include "render/window_backend.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

#include <SDL3/SDL_opengl.h>

namespace ladoga {

namespace {

[[noreturn]] void fail(const char* what) {
    throw std::runtime_error(std::string(what) + ": " + SDL_GetError());
}

MouseButton mouse_button(Uint8 button) {
    switch (button) {
    case SDL_BUTTON_LEFT:
        return MouseButton::Left;
    case SDL_BUTTON_MIDDLE:
        return MouseButton::Middle;
    case SDL_BUTTON_RIGHT:
        return MouseButton::Right;
    default:
        return MouseButton::Left;
    }
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
    SDL_StartTextInput(window_);
}

SDL3Backend::~SDL3Backend() { cleanup(); }

bool SDL3Backend::poll_events(std::vector<Event>& events) {
    SDL_Event sdl_event{};
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
            return false;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (sdl_event.window.windowID == SDL_GetWindowID(window_)) {
                return false;
            }
            break;
        case SDL_EVENT_MOUSE_MOTION: {
            Event event;
            event.type = EventType::MouseMove;
            event.position = {static_cast<int>(sdl_event.motion.x),
                              static_cast<int>(sdl_event.motion.y)};
            events.push_back(std::move(event));
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            Event event;
            event.type = sdl_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? EventType::MouseDown
                                                                       : EventType::MouseUp;
            event.position = {static_cast<int>(sdl_event.button.x),
                              static_cast<int>(sdl_event.button.y)};
            event.button = mouse_button(sdl_event.button.button);
            events.push_back(std::move(event));
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            Event event;
            event.type = EventType::KeyDown;
            event.key = static_cast<int>(sdl_event.key.key);
            events.push_back(std::move(event));
            break;
        }
        case SDL_EVENT_TEXT_INPUT: {
            Event event;
            event.type = EventType::TextInput;
            event.text = sdl_event.text.text != nullptr ? sdl_event.text.text : "";
            events.push_back(std::move(event));
            break;
        }
        default:
            break;
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

void SDL3Backend::set_title(std::string_view title) {
    SDL_SetWindowTitle(window_, std::string(title).c_str());
}

void SDL3Backend::present() { SDL_GL_SwapWindow(window_); }

void SDL3Backend::cleanup() noexcept {
    if (context_ != nullptr) {
        SDL_GL_DestroyContext(context_);
        context_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_StopTextInput(window_);
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

} // namespace ladoga
