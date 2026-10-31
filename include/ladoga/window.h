#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <string_view>

#include <ladoga/widget.h>

namespace ladoga {

/// Корневое окно: заголовок, размер, корень дерева и действия для Click="...".
class Window : public Widget {
public:
    explicit Window(std::string title = "LadogaUI", int width = 800, int height = 600);
    ~Window() override = default;

    const std::string& title() const noexcept;
    void set_title(std::string title);

    // Размер нативного окна. Менять до Application::run().
    void set_size(int width, int height);

    // Создаёт дерево детей из XML. Повторный вызов заменяет дерево.
    void load_from_xml(const std::filesystem::path& path);

    // Событие после hit-test: клик, клавиатура, буксировка мыши.
    bool dispatch_event(const Event& event);

    // Декларативные действия для Click="...".
    void register_action(std::string name, std::function<void()> action);
    bool invoke_action(std::string_view name);

    void layout() override;

protected:
    void layout_children() override;

private:
    void focus_on(Widget* target);

    std::string title_;
    std::map<std::string, std::function<void()>, std::less<>> actions_;
    Widget* mouse_capture_ = nullptr;
    Widget* focused_widget_ = nullptr;
};

} // namespace ladoga
