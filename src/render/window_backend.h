#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <SDL3/SDL.h>

#include <ladoga/core/event.h>
#include <ladoga/core/geometry.h>

namespace ladoga {

/// Абстракция окна и ввода. Ядро не знает, что внутри SDL3.
class WindowBackend {
public:
    virtual ~WindowBackend() = default;

    /// Собирает события. false — окно закрыто, но события из этой пачки ещё валидны.
    virtual bool poll_events(std::vector<Event>& events) = 0;
    virtual Size drawable_size() const = 0;
    virtual void set_title(std::string_view title) = 0;
    virtual void present() = 0;
};

/// SDL3-реализация: создаёт окно, контекст OpenGL и меняет буферы.
class SDL3Backend final : public WindowBackend {
public:
    SDL3Backend(const std::string& title, int width, int height);
    ~SDL3Backend() override;

    SDL3Backend(const SDL3Backend&) = delete;
    SDL3Backend& operator=(const SDL3Backend&) = delete;

    bool poll_events(std::vector<Event>& events) override;
    Size drawable_size() const override;
    void set_title(std::string_view title) override;
    void present() override;

private:
    void cleanup() noexcept;

    SDL_Window* window_ = nullptr;
    SDL_GLContext context_ = nullptr;
};

} // namespace ladoga
