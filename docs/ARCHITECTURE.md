# Архитектура LadogaUI

Документ описывает **внутренние границы**, а не публичный API. Публичный
контракт — в [API_CONTRACT.md](API_CONTRACT.md). Если внутреннее устройство
можно упростить, не ломая контракт, — упрощаем.

## 0. Что реализовано на этапах 4–6

- `Widget::on_event` + `dispatch_event` + `hit_test`; `Window` хранит фокус
  TextBox и захват мыши.
- `Button` вызывает `clicked` только при нажатии и отпускании внутри.
- `TextBox` принимает UTF-8 ввод и Backspace, `CheckBox` переключается,
  `Slider` тянется мышью.
- Текст рисуется через stb_truetype + Roboto в `OpenGLRenderer::draw_text`.
- `03_xml`, `04_events`, `05_hello`, `06_sum` — рабочие примеры с текстом и событиями.
- `XmlLoader` по-прежнему создаёт дерево, `PropertyMap` применяет свойства.
- `OpenGLRenderer` использует fixed-function OpenGL и отдельную текстуру на
  строку; шейдеры/батчинг оставлены на будущее.
- `GridPanel`, темы, плагины и установка — не реализованы.

## 1. Слои

```text
┌─────────────────────────────────────────────────────────┐
│ Публичный API: Widget, Window, StackPanel, Label, ...    │
├─────────────────────────────────────────────────────────┤
│ Ядро: дерево виджетов, layout, hit-test, события         │
├─────────────────────────────────────────────────────────┤
│ Загрузка XML: XmlLoader + WidgetFactory + PropertyMap     │
├─────────────────────────────────────────────────────────┤
│ Абстракции: Renderer, WindowBackend                      │
├─────────────────────────────────────────────────────────┤
│ Платформа: OpenGLRenderer + SDL3Backend                  │
└─────────────────────────────────────────────────────────┘
```

Зависимости идут только вниз. Ядро не знает про SDL3, OpenGL и pugixml.
Платформенные классы не знают про конкретные виджеты.

## 2. Каталоги

```text
include/ladoga/          публичные заголовки
├── ladoga.h             единый заголовок
├── application.h
├── widget.h
├── window.h
├── stack_panel.h
├── label.h, button.h, text_box.h, check_box.h, slider.h
├── renderer.h           абстракция рисования (нужна Widget::paint)
└── core/                геометрия, цвет, margin, события (этап 4)

src/
├── widget.cpp, stack_panel.cpp, window.cpp, виджеты
├── text_metrics.h        грубая оценка ширины текста
├── core/                layout, события
├── render/              WindowBackend, SDL3Backend, OpenGLRenderer, Font
├── xml/                 XmlLoader, WidgetFactory, PropertyMap
└── app/                 Application, цикл событий

third_party/             stb_truetype.h и Roboto (лицензии в README)

examples/                по одному примеру на тему/виджет
tests/                   smoke + поведенческие тесты
docs/                    контракт, справка, план
third_party/             только если зависимость нельзя тянуть через CMake
```

## 3. Владение

- `Application` владеет бэкендом и рендерером на время `run`.
- `Window` — корень дерева и владеет детьми через
  `std::vector<std::unique_ptr<Widget>>`.
- `Widget` хранит невладеющий `Widget* parent`.
- Событийные поля (`clicked` и т. п.) принадлежат виджету.
- `Window::register_action` хранит `std::map<std::string, std::function<void()>>`.
- После `load_from_xml` дерево принадлежит окну; повторная загрузка удаляет
  старые дочерние виджеты.

## 4. Жизненный цикл кадра

Сейчас (этапы 4–6) цикл такой:

```text
window.set_bounds({0, 0, drawable_size})
      ↓
window.layout()
      ↓
WindowBackend::poll_events(events)   // SDL3 → ladoga::Event
      ↓
window.dispatch_event(event) для каждого
      ↓
Renderer::set_viewport / begin_frame
      ↓
window.paint(Renderer&)              // включая текст
      ↓
Renderer::end_frame / SDL_GL_SwapWindow
```

layout пересчитывается каждый кадр — это просто и достаточно для учебных
примеров. Флаг `layout_dirty` появится позже, если понадобится.

- Retained-mode: дерево хранит состояние между кадрами.
- `paint()` не меняет состояние, кроме кэшей.
- Один поток. Никаких мьютексов и очередей между потоками.
- Если окно закрыто, приложение всё равно дорисовывает последний кадр и
  выходит — так проще не терять события, пришедшие вместе с закрытием.

## 5. Раскладка

`Widget::layout()` — виртуальный метод. Базовая реализация:

1. Считает собственный размер, если `Width`/`Height` равны `Auto`.
2. Раскладывает детей (в наследниках, например `StackPanel`).
3. Сохраняет результат в `bounds_`.

`StackPanel`:

- `Vertical`: последовательно сверху вниз, `y += child.height + margin.top +
  margin.bottom`;
- `Horizontal`: последовательно слева направо;
- `Margin` ребёнка учитывается;
- `Auto` по главной оси — по содержимому;
- по поперечной оси ребёнок получает свою ширину/высоту, без растягивания.

Этого достаточно для этапа 2. Растягивание, выравнивание и `GridPanel` —
отдельные шаги.

## 6. События

`WindowBackend` переводит SDL3 в нейтральный `ladoga::Event`:

```cpp
enum class EventType { MouseDown, MouseUp, MouseMove, KeyDown, TextInput };
struct Event {
    EventType type;
    Point position;
    MouseButton button;
    int key;
    std::string text;
};
```

Маршрутизация в `Window::dispatch_event`:

1. `MouseDown`: hit-test, смена фокуса, запоминание `mouse_capture_`.
2. `MouseMove`/`MouseUp`: если есть capture — событие идёт ему, иначе hit-test.
3. `KeyDown`/`TextInput`: событие идёт `focused_widget_` (TextBox).
4. `Widget::dispatch_event`: сначала `on_event`, затем `on_event` предков.
   Повторного hit-test при всплытии нет.

Button хранит `pressed_`. `clicked` вызывается только если нажатие и
отпускание произошли внутри кнопки. CheckBox переключается на MouseUp внутри.
Slider захватывает мышь и обновляет значение по MouseMove.

Удаление виджета во время его события по-прежнему запрещено: это позволяет не
строить сложную систему «безопасных» итераторов.

## 7. XML-загрузка

```text
файл → pugixml → DOM
                 ↓
        XmlLoader (рекурсивный обход)
                 ↓
     WidgetFactory: "Button" → make_unique<Button>()
                 ↓
     PropertyMap: "Text" → &Button::set_text и т. п.
                 ↓
        дерево Widget + Name-индекс
```

Ключевые решения:

- фабрика — `std::map<std::string, std::function<std::unique_ptr<Widget>()>>`;
- свойства — ручная карта `std::map<std::string, PropertySetter>`, без рефлексии;
- `Click` превращается в `clicked`-лямбду, которая ищет действие в окне;
- ошибки — исключение с файлом/строкой/колонкой;
- неизвестный тег/атрибут — ошибка, а не тихий пропуск.

`load_from_xml` живёт в `Window`, но реализация — в `src/xml/`; `Window` только
вызывает загрузчик. Так парсер можно тестировать без окна и OpenGL.

## 8. Рендеринг

`Renderer` — узкий интерфейс, достаточный для этапов 1–5:

```cpp
class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void set_viewport(Size size) = 0;
    virtual void begin_frame(Color clear_color) = 0;
    virtual void draw_rect(const Rect& rect, Color color) = 0;
    virtual void draw_text(const Rect& rect, std::string_view text, Color color) = 0;
    virtual int text_width(std::string_view text) const = 0;
    virtual void end_frame() = 0;
};
```

- Сейчас `OpenGLRenderer` использует fixed-function OpenGL: `glOrtho`,
  `glBegin(GL_QUADS)`. Это осознанное упрощение; шейдеры и батчинг — на будущее.
- `draw_text`: stb_truetype растеризует строку в RGBA-битмап, рендерер
  загружает временную текстуру и рисует один quad. Для учебных примеров этого
  достаточно; атлас и кэш — возможная оптимизация позже.
- Шрифт: `third_party/Roboto-Regular.ttf` (кириллица, Apache 2.0), путь
  `LADOGA_FONT_PATH`; есть fallback на `bin/fonts` и системные шрифты.
- `NullRenderer` для тестов layout/логики без окна пригодится позже.

`WindowBackend` скрывает SDL3:

```cpp
class WindowBackend {
public:
    virtual bool poll_events(std::vector<Event>& events) = 0;
    virtual Size drawable_size() const = 0;
    virtual void set_title(std::string_view title) = 0;
    virtual void present() = 0;
};
```

`SDL3Backend` создаёт окно и контекст OpenGL, переводит SDL-события в
`ladoga::Event`, обновляет заголовок и меняет буферы.

## 9. Зависимости

| Зависимость | Этап | Как подключаем | Зачем |
|---|---|---|---|
| SDL3 3.4.18 | 1 | CMake FetchContent, закреплённый тег | окно, ввод, контекст GL |
| OpenGL | 1 | системный, `find_package(OpenGL)` | рисование |
| pugixml 1.16 | 3 | CMake FetchContent | разбор XML |
| stb_truetype | 5 | header-only, лежит в `third_party` | растеризация текста |
| Roboto Regular | 5 | файл в `third_party` | шрифт с кириллицей |

Правила:

- никаких зависимостей «на будущее»;
- версии закреплены;
- первая сборка требует сети: SDL3 и pugixml скачиваются и собираются статически;
- шрифт и stb_truetype лежат в репозитории, чтобы не зависеть от системных шрифтов;
- лицензии совместимы с учебным и коммерческим использованием
  (SDL3 — zlib, pugixml — MIT, Roboto — Apache 2.0, stb — public domain/MIT,
  OpenGL — системный API).

## 10. Расширяемость (этап 8)

Точки расширения проектируются заранее, но не реализуются раньше времени:

- `WidgetFactory::register_type("MyWidget", ...)` — новый тег без правки ядра;
- `PropertyMap::register_property` — свойства нового виджета;
- `Theme`/`Style` — цвета, отступы, шрифты;
- `Renderer` — альтернативный бэкенд;
- плагины — динамические библиотеки с регистрацией типа.

Главное правило: расширение не должно требовать правки `Widget`, `Window` или
`XmlLoader`. Если требует — архитектура ещё не готова.

## 11. Тестирование

- `tests/smoke_test.cpp` — базовая проверка сборки и типов;
- `tests/layout_test.cpp` — вертикальная/горизонтальная раскладка, `Auto`,
  `find_child`;
- `tests/xml_test.cpp` — загрузка XML, свойства, `Click`, ошибки;
- `tests/events_test.cpp` — hit-test, клик внутри/снаружи, topmost;
- `tests/font_test.cpp` — загрузка шрифта и непустой битмап с кириллицей;
- `tests/widgets_test.cpp` — TextBox, CheckBox, Slider;
- визуальные примеры — отдельно, они не заменяют тесты.

## 12. Осознанные упрощения

- один поток;
- без рефлексии и макросов;
- без dependency properties и биндингов;
- без сложной системы стилей;
- без удаления виджетов во время события;
- текст — через атрибут, без rich text;
- layout — два варианта StackPanel, GridPanel позже.

Каждое упрощение помечается в коде `// TODO: упрощение для этапа N`.
