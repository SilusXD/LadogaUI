# Архитектура LadogaUI

Документ описывает **внутренние границы**, а не публичный API. Публичный
контракт — в [API_CONTRACT.md](API_CONTRACT.md). Если внутреннее устройство
можно упростить, не ломая контракт, — упрощаем.

## 0. Что реализовано на этапе 1

- `Application::run(Window&)` создаёт `SDL3Backend` и `OpenGLRenderer`, крутит
  цикл событий и вызывает `Window::paint(Renderer&)`.
- `Window` пока **не** наследует `Widget`: это отдельный класс с заголовком,
  размером и виртуальным `paint`. Наследование появится на этапе 2.
- `WindowBackend` уже абстракция: ядро не вызывает SDL напрямую. Но `Application`
  пока сам выбирает `SDL3Backend` — внедрение бэкенда появится позже.
- `OpenGLRenderer` рисует fixed-function OpenGL. Это временно (см. TODO в коде).
- XML, `Widget`, `StackPanel`, `Event` и hit-test ещё не реализованы.

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
├── window.h
├── renderer.h           абстракция рисования (нужна Window::paint)
└── core/                геометрия, цвет, margin, события (этапы 2–4)

src/
├── window.cpp
├── core/                Widget, layout, маршрутизация событий (этапы 2–4)
├── render/              WindowBackend, SDL3Backend, OpenGLRenderer
├── xml/                 XmlLoader, WidgetFactory, PropertyMap (этап 3)
└── app/                 Application, цикл событий

examples/                по одному примеру на тему/виджет
tests/                   smoke + поведенческие тесты
docs/                    контракт, справка, план
third_party/             только если зависимость нельзя тянуть через CMake
```

## 3. Владение

- `Application` владеет бэкендом и рендерером на время `run`.
- На этапе 1 `Window` не владеет детьми: дерева ещё нет. С этапа 2 `Window`
  станет корнем дерева и будет владеть детьми через
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

На этапе 1 цикл короткий:

```text
poll SDL events (закрытие окна)
      ↓
Renderer::set_viewport(drawable_size)
      ↓
Renderer::begin_frame(clear_color)
      ↓
Window::paint(Renderer&)
      ↓
Renderer::end_frame()
      ↓
SDL_GL_SwapWindow
```

С этапа 2–4 сюда добавятся layout, hit-test и маршрутизация событий:

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
    virtual void set_viewport(Size size) = 0;
    virtual void begin_frame(Color clear_color) = 0;
    virtual void draw_rect(const Rect& rect, Color color) = 0;
    virtual void draw_text(const Rect& rect, std::string_view text, Color color) = 0;
    virtual void end_frame() = 0;
};
```

- На этапе 1 `OpenGLRenderer` использует fixed-function OpenGL: `glOrtho`,
  `glBegin(GL_QUADS)`. Это осознанное упрощение; шейдеры и батчинг — этап 5.
- `Renderer` скрывает OpenGL от `Window::paint`.
- Текст на этапе 5: отдельный шаг выбора шрифтового бэкенда (stb_truetype +
  свободный шрифт или SDL3_ttf). До этого `draw_text` — заглушка.
- `NullRenderer` для тестов layout/логики без окна — полезен на этапах 2–3.

`WindowBackend` скрывает SDL3:

```cpp
class WindowBackend {
public:
    virtual ~WindowBackend() = default;
    virtual bool poll_events() = 0;
    virtual Size drawable_size() const = 0;
    virtual void present() = 0;
};
```

`SDL3Backend` создаёт окно и контекст OpenGL. `Application` получает его
напрямую и вызывает `poll_events`, `drawable_size`, `present`.

## 9. Зависимости

| Зависимость | Этап | Как подключаем | Зачем |
|---|---|---|---|
| SDL3 3.4.18 | 1 | CMake FetchContent, закреплённый тег | окно, ввод, контекст GL |
| OpenGL | 1 | системный, `find_package(OpenGL)` | рисование |
| pugixml | 3 | FetchContent или vendored MIT-код | разбор XML |
| шрифтовой бэкенд | 5 | отдельное решение | текст и кириллица |

Правила:

- никаких зависимостей «на будущее»;
- версии закреплены;
- первая сборка требует сети: SDL3 скачивается и собирается статически;
- лицензии совместимы с учебным и коммерческим использованием
  (SDL3 — zlib, OpenGL — системный API).

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
