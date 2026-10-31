# Справочник API

Публичные классы `namespace ladoga`. Заголовок для студента:

```cpp
#include <ladoga/ladoga.h>
```

## Application

```cpp
class Application {
public:
    Application(int argc, char** argv);
    int run(Window& window);
};
```

Создаёт окно SDL3, контекст OpenGL, цикл событий и рендерер. Возвращает 0 после
закрытия окна.

```cpp
int main(int argc, char** argv) {
    ladoga::Application app(argc, argv);
    MyWindow window;
    return app.run(window);
}
```

## Window

```cpp
class Window : public Widget {
public:
    explicit Window(std::string title = "LadogaUI", int width = 800, int height = 600);

    const std::string& title() const;
    void set_title(std::string title);
    void set_size(int width, int height);

    void load_from_xml(const std::filesystem::path& path);
    bool dispatch_event(const Event& event);

    void register_action(std::string name, std::function<void()> action);
    bool invoke_action(std::string_view name);
};
```

- `load_from_xml` заменяет дерево детей; корневой тег — `<Window>`.
- `register_action` связывает `Click="onOk"` с C++-лямбдой.
- `set_title` обновляет заголовок нативного окна в следующем кадре.

## Widget

```cpp
class Widget {
public:
    static constexpr int Auto = -1;

    const std::string& name() const;
    void set_name(std::string value);

    int width() const;
    int height() const;
    void set_width(int value);
    void set_height(int value);

    Margin margin() const;
    void set_margin(Margin value);
    void set_margin(int value);

    bool visible() const;
    void set_visible(bool value);

    Rect bounds() const;
    void set_bounds(const Rect& value);

    Widget* parent() const;
    const std::vector<std::unique_ptr<Widget>>& children() const;
    void add_child(std::unique_ptr<Widget> child);

    Widget* find_child(std::string_view name);
    template <typename T> T* find_child(std::string_view name);

    virtual Size preferred_size() const;
    virtual void layout();
    virtual void paint(Renderer& renderer);

    virtual bool on_event(const Event& event);
    virtual bool dispatch_event(const Event& event);
    virtual Widget* hit_test(Point point);
};
```

`Width`/`Height` могут быть `Auto`. `Margin` — внешний отступ, `bounds()` —
результат раскладки.

## StackPanel

```cpp
enum class Orientation { Vertical, Horizontal };

class StackPanel : public Widget {
public:
    Orientation orientation() const;
    void set_orientation(Orientation value);
};
```

```xml
<StackPanel Orientation="Vertical" Margin="16"> ... </StackPanel>
```

## Label

```cpp
class Label : public Widget {
public:
    const std::string& text() const;
    void set_text(std::string value);
};
```

## Button

```cpp
class Button : public Widget {
public:
    const std::string& text() const;
    void set_text(std::string value);

    std::function<void()> clicked;
};
```

`clicked` вызывается после нажатия и отпускания внутри кнопки. `Click` в XML
создаёт этот обработчик через реестр действий окна.

## TextBox

```cpp
class TextBox : public Widget {
public:
    const std::string& text() const;
    void set_text(std::string value);

    std::function<void(const std::string&)> text_changed;
};
```

Клик ставит фокус. Печатный текст добавляется в конец строки; Backspace удаляет
последний UTF-8 символ. Программный `set_text` событие не вызывает.

## CheckBox

```cpp
class CheckBox : public Widget {
public:
    const std::string& text() const;
    void set_text(std::string value);

    bool checked() const;
    void set_checked(bool value);

    std::function<void(bool)> checked_changed;
};
```

## Slider

```cpp
class Slider : public Widget {
public:
    int value() const;
    int min() const;
    int max() const;

    void set_min(int value);
    void set_max(int value);
    void set_range(int min, int max);
    void set_value(int value);

    std::function<void(int)> value_changed;
};
```

## Базовые типы

```cpp
struct Point { int x; int y; };
struct Size { int width; int height; };

struct Rect {
    int x, y, width, height;
    int left() const;
    int top() const;
    int right() const;
    int bottom() const;
    bool empty() const;
    bool contains(Point point) const;
};

struct Color { std::uint8_t r, g, b, a; };
struct Margin { int left, top, right, bottom; };
```

`Rect::contains` не включает правый и нижний край.

## Renderer

```cpp
class Renderer {
public:
    virtual void set_viewport(Size size) = 0;
    virtual void begin_frame(Color clear_color) = 0;
    virtual void draw_rect(const Rect& rect, Color color) = 0;
    virtual void draw_text(const Rect& rect, std::string_view text, Color color) = 0;
    virtual int text_width(std::string_view text) const = 0;
    virtual void end_frame() = 0;
};
```

`Window::paint(Renderer&)` рисует через этот интерфейс. `OpenGLRenderer` —
реализация на OpenGL.

## События

```cpp
enum class EventType { MouseDown, MouseUp, MouseMove, KeyDown, TextInput };
enum class MouseButton { Left, Middle, Right };

struct Event {
    EventType type;
    Point position;
    MouseButton button;
    int key;
    std::string text;
};
```

`Window::dispatch_event` делает hit-test, ставит фокус TextBox и отправляет
событие нужному виджету. `Button`, `CheckBox`, `TextBox` и `Slider`
обрабатывают события внутри `on_event`.

## Соответствие Qt/WPF

| LadogaUI | Qt | WPF |
|---|---|---|
| `Widget` | `QWidget` | `FrameworkElement` |
| `Window` | `QMainWindow` | `Window` |
| `StackPanel` | `QVBoxLayout` / `QHBoxLayout` | `StackPanel` |
| `Label` | `QLabel` | `Label` |
| `Button` | `QPushButton` | `Button` |
| `TextBox` | `QLineEdit` | `TextBox` |
| `CheckBox` | `QCheckBox` | `CheckBox` |
| `Slider` | `QSlider` | `Slider` |
| `clicked` | `clicked` signal | `Click` event |
