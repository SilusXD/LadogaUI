#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <ladoga/core/geometry.h>
#include <ladoga/core/margin.h>
#include <ladoga/renderer.h>

namespace ladoga {

struct Event;

/// База дерева виджетов: имя, размеры, отступы, дети и рисование.
class Widget {
public:
    /// Width/Height = Auto: размер по содержимому.
    static constexpr int Auto = -1;

    Widget() = default;
    virtual ~Widget() = default;

    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    const std::string& name() const noexcept;
    void set_name(std::string value);

    int width() const noexcept;
    int height() const noexcept;
    void set_width(int value);
    void set_height(int value);

    Margin margin() const noexcept;
    void set_margin(Margin value);
    void set_margin(int value);

    bool visible() const noexcept;
    void set_visible(bool value);

    Rect bounds() const noexcept;
    void set_bounds(const Rect& value);

    Widget* parent() const noexcept;
    const std::vector<std::unique_ptr<Widget>>& children() const noexcept;
    void add_child(std::unique_ptr<Widget> child);

    Widget* find_child(std::string_view name) noexcept;
    const Widget* find_child(std::string_view name) const noexcept;

    template <typename T>
    T* find_child(std::string_view name) noexcept {
        return dynamic_cast<T*>(find_child(name));
    }

    template <typename T>
    const T* find_child(std::string_view name) const noexcept {
        return dynamic_cast<const T*>(find_child(name));
    }

    /// Размер, который виджет хочет получить, без учёта Margin.
    virtual Size preferred_size() const;
    virtual void layout();
    virtual void paint(Renderer& renderer);

    // События: hit-test, поглощение и всплытие к родителю.
    virtual bool on_event(const Event& event);
    virtual bool dispatch_event(const Event& event);
    virtual Widget* hit_test(Point point);

    // Фокус нужен TextBox; остальные виджеты его не принимают.
    [[nodiscard]] virtual bool accepts_focus() const;
    virtual void set_focus(bool focused);

protected:
    virtual void layout_children();
    virtual void paint_self(Renderer& renderer);
    void clear_children() noexcept;

private:
    std::string name_;
    int width_ = Auto;
    int height_ = Auto;
    Margin margin_;
    bool visible_ = true;
    Rect bounds_;
    Widget* parent_ = nullptr;
    std::vector<std::unique_ptr<Widget>> children_;
};

} // namespace ladoga
