# Контракт публичного API (этап 0)

Статус: **контракт зафиксирован, реализация идёт**. Этапы 1–3 закрыли
`Application`, `Window`, `Widget`, `StackPanel`, пять виджетов и XML-загрузку.
События — этап 4, текст и ввод — этап 5. Любое изменение имён, свойств или
XML-синтаксиса — это изменение контракта: сначала правим этот файл и
`PROJECT_BRIEF.md`, потом код.

## 1. Пространство имён и подключение

Всё публичное лежит в `namespace ladoga`.

```cpp
#include <ladoga/ladoga.h>
```

`ladoga.h` — единый заголовок для студента. Точечные заголовки
(`<ladoga/button.h>` и т. п.) тоже допустимы, но в примерах используем единый.

## 2. Иерархия классов

```text
Widget
├── Window          корневое окно: заголовок, размер, загрузка XML
├── StackPanel      линейная раскладка детей
├── GridPanel       строки/столбцы (позже, этап 5+)
├── Label           текст только для чтения
├── Button          кликабельная кнопка
├── TextBox         ввод одной строки
├── CheckBox        флаг вкл/выкл
└── Slider          числовой диапазон
```

`Application` — не виджет: инициализирует бэкенд и запускает цикл событий.

## 3. Базовый Widget

```cpp
class Widget {
public:
    static constexpr int Auto = -1; // Width/Height: "размер по содержимому"

    virtual ~Widget() = default;

    // Name
    const std::string& name() const noexcept;
    void set_name(std::string value);

    // Width / Height
    int width() const noexcept;              // Auto или пиксели
    int height() const noexcept;
    void set_width(int px);                  // px >= 0 или Widget::Auto
    void set_height(int px);

    // Margin: один int на все стороны или четыре: left top right bottom
    Margin margin() const noexcept;
    void set_margin(Margin value);
    void set_margin(int all);

    // Visible
    bool visible() const noexcept;
    void set_visible(bool value);

    // Геометрия, которую посчитал layout()
    Rect bounds() const noexcept;
    void set_bounds(const Rect& value);

    // Размер, который виджет хочет получить, без Margin
    virtual Size preferred_size() const;

    // Дерево
    Widget* parent() const noexcept;
    const std::vector<std::unique_ptr<Widget>>& children() const noexcept;
    void add_child(std::unique_ptr<Widget> child);

    Widget* find_child(std::string_view name) noexcept;

    template <typename T>
    T* find_child(std::string_view name) noexcept;

    // Переопределяются наследниками
    virtual void layout();
    virtual void paint(Renderer& renderer);
    virtual bool on_event(const Event& event);
};
```

Правила:

- `find_child` ищет по `Name` во всём поддереве, а не только среди прямых детей.
- `find_child<T>` возвращает `nullptr`, если имя не найдено или тип не совпал.
- `add_child` передаёт владение; родитель хранит `std::unique_ptr<Widget>`.
- `Auto` для `Width`/`Height` означает «размер по содержимому». Для `Window`
  отсутствие `Width`/`Height` даёт размер по умолчанию 800×600.
- `layout()` вызывается сверху вниз, `paint()` — после layout.
- `on_event()` возвращает `true`, если событие поглощено. Появится на этапе 4;
  в текущем `Widget` его ещё нет.

### Margin

```cpp
struct Margin {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    static constexpr Margin all(int value) noexcept;
};
```

`set_margin(16)` — все стороны по 16. `set_margin({16, 8, 16, 8})` — явно.
Отрицательные значения запрещены.

## 4. Window

```cpp
class Window : public Widget {
public:
    Window();

    const std::string& title() const noexcept;
    void set_title(std::string value);

    // Размер корневого окна. По умолчанию 800×600.
    void set_size(int width, int height);

    // Создаёт дерево детей из XML. Повторный вызов заменяет дерево.
    void load_from_xml(const std::filesystem::path& path);

    // Декларативные действия для Click="..."
    void register_action(std::string name, std::function<void()> action);
    bool invoke_action(std::string_view name);
};
```

**Состояние этапа 3.** `Window` уже наследует `Widget`, загружает XML, хранит
действия и вызывает их через `invoke_action`. `Click` создаёт `Button::clicked`,
но hit-test и реальные клики появятся на этапе 4. Текст в виджетах — этап 5,
поэтому сейчас `Text` хранится, но на экране не рисуется.

`load_from_xml` вызывается у уже существующего окна: корневой тег `<Window>`
не создаёт второе окно, а применяет атрибуты к `this` и строит детей.

Ошибки загрузки — исключение `std::runtime_error` с сообщением вида
`main.xml:12:5: неизвестный тег <Buton>`. Файл читается как UTF-8.

## 5. Application

```cpp
class Application {
public:
    Application(int argc, char** argv);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    int run(Window& window);
};
```

`run` создаёт нативное окно, запускает цикл событий и возвращает код выхода.
Цикл однопоточный. Закрытие окна завершает `run`.

## 6. Конкретные виджеты

### StackPanel

```cpp
enum class Orientation { Vertical, Horizontal };

class StackPanel : public Widget {
public:
    Orientation orientation() const noexcept;      // по умолчанию Vertical
    void set_orientation(Orientation value);
};
```

Дети располагаются по порядку. `Margin` ребёнка учитывается. `Auto`-размер
ребёнка по главной оси — по содержимому, по поперечной — растягивание не
делаем: ширина/высота по содержимому.

### Label

```cpp
class Label : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);
};
```

### Button

```cpp
class Button : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);

    std::function<void()> clicked;
};
```

`clicked` вызывается по отпусканию кнопки мыши внутри кнопки, если нажатие
тоже было внутри. Это поведение Qt/WPF, а не «клик по любому нажатию».

### TextBox

```cpp
class TextBox : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);

    std::function<void(const std::string&)> text_changed;
};
```

`text_changed` вызывается после изменения текста пользователем. Программный
`set_text` событие не порождает.

### CheckBox

```cpp
class CheckBox : public Widget {
public:
    const std::string& text() const noexcept;
    void set_text(std::string value);

    bool checked() const noexcept;
    void set_checked(bool value);

    std::function<void(bool)> checked_changed;
};
```

### Slider

```cpp
class Slider : public Widget {
public:
    int value() const noexcept;
    int min() const noexcept;
    int max() const noexcept;

    void set_min(int value);
    void set_max(int value);
    void set_range(int min, int max);
    void set_value(int value); // зажимается в [min, max]

    std::function<void(int)> value_changed;
};
```

`set_value` программно событие не порождает; пользовательское движение —
порождает.

## 7. События

Публичный API использует `std::function`, а не сигналы/слоты:

| Виджет | Поле | Когда вызывается |
|---|---|---|
| Button | `std::function<void()> clicked` | Клик завершён |
| TextBox | `std::function<void(const std::string&)> text_changed` | Пользователь изменил текст |
| CheckBox | `std::function<void(bool)> checked_changed` | Пользователь переключил флаг |
| Slider | `std::function<void(int)> value_changed` | Пользователь изменил значение |

Правила:

- Поля публичные, назначаются лямбдой: `button.clicked = [this] { ... };`.
- Пустой `std::function` — нормальное состояние, вызов не требуется.
- Обработчик вызывается после изменения состояния виджета.
- Обработчики не должны удалять виджет из дерева внутри самого обработчика.
  Это ограничение этапа 4; позже можно ослабить.
- Внутри обработчика можно менять свойства других виджетов и вызывать
  `set_text`, `set_checked` и т. п.

## 8. XML → C++: имена и действия

`Click="onOk"` — это имя действия, зарегистрированного в окне:

```cpp
window.register_action("onOk", [&] { onOk(); });
```

Во время загрузки XML атрибут `Click` превращается в `clicked`-лямбду,
которая во время клика ищет действие по имени. Поэтому порядок регистрации и
загрузки не важен:

```cpp
load_from_xml("main.xml");                    // Click="onOk" уже разобран
register_action("onOk", [this] { onOk(); });  // можно и после
```

Прямое назначение `okButton().clicked = ...` перекрывает XML-привязку.
Если действие не зарегистрировано, клик пишет предупреждение в `stderr` и
ничего не делает. Это лучше, чем молчаливый «мёртвый» интерфейс.

## 9. Доступ к виджетам по имени

Отдельного генератора доступа нет: студент пишет короткие методы-обёртки.

```cpp
class MainWindow : public ladoga::Window {
public:
    MainWindow() {
        load_from_xml("main.xml");
        register_action("onOk", [this] { onOk(); });
    }

    ladoga::Button& okButton() { return *find_child<ladoga::Button>("okButton"); }
    ladoga::Label& greeting() { return *find_child<ladoga::Label>("greeting"); }
    ladoga::TextBox& nameBox() { return *find_child<ladoga::TextBox>("nameBox"); }

private:
    void onOk() {
        greeting().set_text("Привет, " + nameBox().text());
    }
};
```

Это осознанное решение: без рефлексии и макросов. Студент видит, откуда
берётся `okButton()`, и может сделать такой же метод для любого Name.

## 10. Полный пример программы

```cpp
#include <ladoga/ladoga.h>

class MainWindow : public ladoga::Window {
public:
    MainWindow() {
        load_from_xml("main.xml");
        register_action("onOk", [this] { onOk(); });
    }

    ladoga::Button& okButton() { return *find_child<ladoga::Button>("okButton"); }
    ladoga::Label& greeting() { return *find_child<ladoga::Label>("greeting"); }
    ladoga::TextBox& nameBox() { return *find_child<ladoga::TextBox>("nameBox"); }

private:
    void onOk() {
        greeting().set_text("Привет, " + nameBox().text());
    }
};

int main(int argc, char** argv) {
    ladoga::Application app(argc, argv);
    MainWindow window;
    return app.run(window);
}
```

Альтернатива без `Click` в XML:

```cpp
load_from_xml("main.xml");
okButton().clicked = [this] { onOk(); };
```

Оба варианта должны работать. Первый ближе к WPF, второй — к Qt.

## 11. Что считается публичным API

Публично всё, что лежит в `include/ladoga/` и описано в этом документе.
`Renderer`, `WindowBackend`, фабрика и парсер — техническая часть; студент
может их не трогать. Расширение через них появится на этапе 8.

## 12. Отложено сознательно

- сигналы/слоты, dependency properties, биндинги;
- `GridPanel`;
- стили и темы;
- шрифты, ввод с клавиатуры, курсор в TextBox;
- плагины и установка;
- сети, БД, многопоточность, анимации.

Каждый пункт получает отдельный шаг в `docs/PLAN.md`, а не «заодно».
