# LadogaUI

Учебный C++ GUI-фреймворк с декларативной XML-разметкой. По именам классов,
свойств и событий он должен быть узнаваем для тех, кто позже откроет Qt или WPF.

Главная цель — не «ещё один GUI-движок», а понятный учебник: студент читает
`<Button Text="OK" Click="onOk"/>` и понимает, что произойдёт. Внутреннее
устройство (парсер, фабрика, маршрутизация событий) вторично и скрыто.

## Статус

**Этапы 2–3 — дерево виджетов, раскладка и XML.** Работает:

- `Widget`, дерево детей, `Name`, `Width`/`Height`, `Margin`, `Visible`;
- `StackPanel` с `Orientation="Vertical"` и `Orientation="Horizontal"`;
- виджеты `Label`, `Button`, `TextBox`, `CheckBox`, `Slider` — пока в виде
  цветных заглушек, без текста;
- XML-загрузка через pugixml 1.16: фабрика по тегу, карта свойств, ошибки с
  файлом/строкой/колонкой;
- `Window::load_from_xml`, `Click="onOk"` и реестр действий;
- примеры `01_window`, `02_layout`, `03_xml`;
- тесты `smoke`, `layout`, `xml`.

Текст, ввод и клики мышью — этапы 4–5. Сейчас `Click` только сохраняет действие;
hit-test и вызов по клику появятся на этапе 4.

## Требования

- C++20;
- CMake 3.24+;
- компилятор: MSVC 2022+, GCC 12+, Clang 15+;
- OpenGL-драйвер;
- интернет при первой сборке: CMake скачивает SDL3 3.4.18 и pugixml 1.16.

Если SDL3 или pugixml уже установлены в системе, можно собирать с
`-DLADOGA_USE_SYSTEM_SDL3=ON` и `-DLADOGA_USE_SYSTEM_PUGIXML=ON`.

## Сборка

### В CLion

1. Открыть папку `LadogaUI`.
2. `File → Reload CMake Project` (или согласиться на перезагрузку).
3. В списке конфигураций запуска выбрать **ladoga_03_xml** и нажать Run.
4. Для тестов: в терминале CLion выполнить `ctest --test-dir cmake-build-debug --output-on-failure`.

Первая настройка CMake скачивает и собирает SDL3 и pugixml — это занимает несколько минут.
Все примеры и тесты собираются в `<build>/bin`:
`cmake-build-debug/bin/ladoga_03_xml.exe`,
`cmake-build-debug/bin/ladoga_02_layout.exe`,
`cmake-build-debug/bin/ladoga_01_window.exe`,
`cmake-build-debug/bin/ladoga_00_smoke.exe`,
`cmake-build-debug/bin/ladoga_smoke_test.exe`,
`cmake-build-debug/bin/ladoga_layout_test.exe`,
`cmake-build-debug/bin/ladoga_xml_test.exe`.

### В терминале Windows

Если `cmake`, `ninja` и компилятор уже в `PATH`:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\ladoga_03_xml.exe
```

Ожидаемо: окно 520×420 с заголовком `LadogaUI - XML`, внутри — цветные
заглушки Label, TextBox, Button, CheckBox и Slider. Текст ещё не рисуется.
Окно закрывается крестиком, процесс завершается с кодом 0.

На этой машине компилятора в `PATH` нет, поэтому можно временно добавить
инструменты, встроенные в CLion:

```powershell
$clion = "C:\Programming\Apps\JetBrains\CLion 2026.2.4.1"
$env:PATH = "$clion\bin\cmake\win\x64\bin;$clion\bin\ninja\win\x64;$clion\bin\mingw\bin;$env:PATH"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\ladoga_03_xml.exe
```

### В терминале Linux / macOS

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/bin/ladoga_03_xml
```

## Структура

```text
LadogaUI/
├── CMakeLists.txt          сборка ядра, примеров и тестов
├── PROJECT_BRIEF.md        исходный бриф проекта — источник истины
├── AGENTS.md               правила для ИИ-агента и точка входа в проект
├── cmake/Dependencies.cmake SDL3, OpenGL, pugixml
├── include/ladoga/         публичный API (то, что видит студент)
│   ├── ladoga.h            единый заголовок
│   ├── application.h       точка входа
│   ├── widget.h            база дерева
│   ├── window.h            окно и load_from_xml
│   ├── stack_panel.h       вертикальная/горизонтальная панель
│   ├── label.h, button.h, text_box.h, check_box.h, slider.h
│   ├── renderer.h          абстракция рисования
│   └── core/               геометрия, цвет, отступы
├── src/
│   ├── app/                Application и цикл событий
│   ├── render/             SDL3Backend, OpenGLRenderer, WindowBackend
│   ├── xml/                XmlLoader, WidgetFactory, PropertyMap
│   └── widget.cpp, stack_panel.cpp, window.cpp, виджеты
├── examples/               01_window, 02_layout, 03_xml
├── tests/                  smoke, layout, xml
└── docs/
    ├── API_CONTRACT.md     зафиксированные имена классов, свойств и событий
    ├── XML_REFERENCE.md    синтаксис разметки
    ├── ARCHITECTURE.md     внутренние границы модулей
    ├── ROADMAP.md          этапы 0–8
    ├── PLAN.md             план из маленьких шагов
    └── QUICKSTART.md       быстрый старт
```

## Документация

- [Контракт API](docs/API_CONTRACT.md)
- [Справочник по XML](docs/XML_REFERENCE.md)
- [Архитектура](docs/ARCHITECTURE.md)
- [Дорожная карта](docs/ROADMAP.md)
- [Подробный план](docs/PLAN.md)
- [Быстрый старт](docs/QUICKSTART.md)
- [Исходный бриф](PROJECT_BRIEF.md)

## Правила проекта

1. Публичный API и документация первичны; внутренности можно упрощать.
2. Не добавлять сети, базы данных, многопоточность и сложные анимации.
3. Каждый новый виджет сопровождается примером и страницей справки.
4. При конфликте «красиво» и «понятно студенту» выбирать второе.
5. Временные упрощения помечать комментарием `// TODO: упрощение для этапа N`.
