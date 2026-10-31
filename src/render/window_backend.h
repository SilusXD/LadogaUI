#pragma once

#include <string>

#include <SDL3/SDL.h>

#include <ladoga/core/geometry.h>

namespace ladoga {

/// Абстракция окна и ввода. Ядро не знает, что внутри SDL3.
class WindowBackend {
public:
    virtual ~WindowBackend() = default;

    /// Обрабатывает накопленные события. false — окно закрыто.
    virtual bool poll_events() = 0;
    virtual Size drawable_size() const = 0;
    virtual void present() = 0;
};

/// SDL3-реализация: создаёт окно, контекст OpenGL и меняет буферы.
class SDL3Backend final : public WindowBackend {
public:
    SDL3Backend(const std::string& title, int width, int height);
    ~SDL3Backend() override;

    SDL3Backend(const SDL3Backend&) = delete;
    SDL3Backend& operator=(const SDL3Backend&) = delete;

    bool poll_events() override;
    Size drawable_size() const override;
    void present() override;

private:
    void cleanup() noexcept;

    SDL_Window* window_ = nullptr;
    SDL_GLContext context_ = nullptr;
};

} // namespace ladoga
