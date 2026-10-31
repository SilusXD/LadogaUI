# Архитектура LadogaUI

Документ описывает **внутренние границы**, а не публичный API. Публичный
контракт — в [API_CONTRACT.md](API_CONTRACT.md). Если внутреннее устройство
можно упростить, не ломая контракт, — упрощаем.

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
├── label.h
├── button.h
├── text_box.h
├── check_box.h
├── slider.h
├── renderer.h           абстракция рисования (нужна Widget::paint)
└── core/                геометрия, цвет, события, margin

src/
├── core/                Widget, layout, маршрутизация событий
├── render/              SDL3Backend, OpenGLRenderer, шейдеры
├── xml/                 XmlLoader, WidgetFactory, PropertyMap
└── app/                 Application, цикл событий

examples/                по одному примеру на тему/виджет
tests/                   smoke + поведенческие тесты
docs/                    контракт, справка, план
third_party/             только если зависимость нельзя тянуть через CMake
```

## 3. Владение

- `Application` владеет бэкендом и рендерером на время `run`.
- `Window` — корень дерева. Он владеет детьми через
  `std::vector<std::unique_ptr<Widget>>`.
- `Widget` хранит невладеющий `Widget* parent`.
- Событийные поля (`clicked` и т. п.) принадлежат виджету.
- `Window::register_action` хранит `std::map<std::string, std::function<void()>>`.
- После `load_from_xml` дерево принадлежит окну; повторная загрузка удаляет
  старые дочерние виджеты.

Удаление виджета во время обработки его события запрещено на этапе 4: это
позволяет не строить сложную систему «безопасных» итераторов. Ограничение
задокументировано в контракте.

## 4. Жизненный цикл кадра

```text
poll SDL events
      ↓
перевод в ladoga::Event
      ↓
hit-test + on_event() по дереву
      ↓
если layout_dirty: layout()
      ↓
paint(root, Renderer&)
      ↓
Renderer::present()
```

- Retained-mode: дерево хранит состояние между кадрами.
- `layout()` пересчитывается только когда меняются размеры/свойства/дерево
  (флаг `layout_dirty`).
- `paint()` не меняет состояние, кроме кэшей.
- Один поток. Никаких мьютексов и очередей между потоками.

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

`WindowBackend` отдаёт платформенные события в нейтральном виде:

```cpp
struct MouseEvent { Point position; MouseButton button; };
struct KeyEvent   { int key; bool pressed; };
using Event = std::variant<MouseEvent, KeyEvent, CloseEvent, ResizeEvent>;
```

Точная форма уточняется на этапе 4. Маршрутизация:

1. Hit-test идёт по дереву сверху вниз, ищет самый верхний видимый виджет,
   содержащий точку.
2. Событие предлагается виджету.
3. Если `on_event` вернул `false`, событие поднимается к родителю.
4. `Window` — последний получатель.

Button хранит `pressed_inside_`. `clicked` вызывается только если нажатие и
отпускание произошли внутри кнопки.

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
    virtual void begin_frame(Color clear_color) = 0;
    virtual void draw_rect(const Rect& rect, Color color) = 0;
    virtual void draw_text(const Rect& rect, std::string_view text, Color color) = 0;
    virtual void end_frame() = 0;
};
```

- `OpenGLRenderer` реализует интерфейс через шейдеры и батчинг прямоугольников.
- Текст на этапе 5: отдельный шаг выбора шрифтового бэкенда (stb_truetype +
  свободный шрифт или SDL3_ttf). До этого `draw_text` может быть заглушкой.
- `NullRenderer` для тестов layout/логики без окна — полезен на этапах 2–3.

`WindowBackend` скрывает SDL3:

```cpp
class WindowBackend {
public:
    virtual ~WindowBackend() = default;
    virtual bool poll_event(Event& event) = 0;
    virtual Size drawable_size() const = 0;
    virtual void swap_buffers() = 0;
};
```

`SDL3Backend` создаёт окно и контекст OpenGL. Ядро получает только
`WindowBackend&` и `Renderer&`.

## 9. Зависимости

| Зависимость | Этап | Как подключаем | Зачем |
|---|---|---|---|
| SDL3 | 1 | CMake FetchContent, закреплённый тег | окно, ввод, контекст GL |
| OpenGL | 1 | системный + загрузчик функций | рисование |
| pugixml | 3 | FetchContent или vendored MIT-код | разбор XML |
| шрифтовой бэкенд | 5 | отдельное решение | текст и кириллица |

Правила:

- никаких зависимостей «на будущее»;
- версии закреплены;
- на этапе 0 проект собирается без сети и без внешних библиотек;
- лицензии совместимы с учебным и коммерческим использованием.

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
- этап 2: тесты layout без окна (`NullRenderer`);
- этап 3: тесты парсера на строках XML, без файлов и GL;
- этап 4: тесты hit-test и клика без реального окна;
- этап 5: тесты свойств каждого виджета;
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
