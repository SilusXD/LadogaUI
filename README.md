# LadogaUI

Учебный C++ GUI-фреймворк с декларативной XML-разметкой. По именам классов,
свойств и событий он должен быть узнаваем для тех, кто позже откроет Qt или WPF.

Главная цель — не «ещё один GUI-движок», а понятный учебник: студент читает
`<Button Text="OK" Click="onOk"/>` и понимает, что произойдёт. Внутреннее
устройство (парсер, фабрика, маршрутизация событий) вторично и скрыто.

## Статус

**Этап 0 — контракт и каркас.** Сейчас в репозитории есть:

- CMake-проект на C++20 без внешних зависимостей;
- публичные заголовки `ladoga/version.h`, `ladoga/core/geometry.h`, `ladoga/core/color.h`;
- smoke-пример и smoke-тест;
- зафиксированный контракт API и XML;
- дорожная карта и подробный план из маленьких шагов.

SDL3, OpenGL, pugixml и виджеты появятся на следующих этапах. Пока `ladoga_00_smoke`
ничего не рисует — он проверяет, что каркас собирается.

## Требования

- C++20;
- CMake 3.24+;
- компилятор: MSVC 2022+, GCC 12+, Clang 15+;
- для следующих этапов: SDL3, OpenGL, pugixml (подключаются через CMake, не вручную).

## Сборка

### В CLion

1. Открыть папку `LadogaUI`.
2. `File → Reload CMake Project` (или согласиться на перезагрузку).
3. В списке конфигураций запуска выбрать **LadogaUI** (или `ladoga_00_smoke` —
   это цель CMake и её путь на диске `bin/ladoga_00_smoke.exe`) и нажать Run.
4. Для тестов: в терминале CLion выполнить `ctest --test-dir cmake-build-debug --output-on-failure`.

Все примеры и тесты собираются в `<build>/bin`: `cmake-build-debug/bin/ladoga_00_smoke.exe`,
`cmake-build-debug/bin/ladoga_smoke_test.exe`.

### В терминале Windows

Если `cmake`, `ninja` и компилятор уже в `PATH`:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\ladoga_00_smoke.exe
```

На этой машине компилятора в `PATH` нет, поэтому можно временно добавить
инструменты, встроенные в CLion:

```powershell
$clion = "C:\Programming\Apps\JetBrains\CLion 2026.2.4.1"
$env:PATH = "$clion\bin\cmake\win\x64\bin;$clion\bin\ninja\win\x64;$clion\bin\mingw\bin;$env:PATH"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\ladoga_00_smoke.exe
```

### В терминале Linux / macOS

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/bin/ladoga_00_smoke
```

## Структура

```text
LadogaUI/
├── CMakeLists.txt          сборка ядра, примеров и тестов
├── PROJECT_BRIEF.md        исходный бриф проекта — источник истины
├── AGENTS.md               правила для ИИ-агента и точка входа в проект
├── include/ladoga/         публичный API (то, что видит студент)
│   ├── ladoga.h            единый заголовок
│   └── core/               базовые типы: геометрия, цвет
├── src/                    реализация ядра
├── examples/               примеры; каждый новый виджет = новый пример
├── tests/                  smoke-тесты и будущие тесты поведения
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
