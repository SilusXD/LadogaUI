# Быстрый старт

Статус: **этап 1.** Работает окно SDL3 + OpenGL с цветным прямоугольником.
Виджеты и XML появятся на этапах 2–5; полноценный Quickstart перепишем на этапе 6.

## 1. Что нужно

- Windows, Linux или macOS;
- CMake 3.24+;
- компилятор C++20: MSVC 2022+, GCC 12+ или Clang 15+;
- OpenGL-драйвер;
- интернет при первой сборке: CMake скачивает SDL3 3.4.18.

SDL3 не нужно ставить вручную. Если он уже есть в системе, можно передать
`-DLADOGA_USE_SYSTEM_SDL3=ON`.

## 2. Сборка в CLion

1. Открыть папку `LadogaUI`.
2. `File → Reload CMake Project`.
3. Дождаться первой настройки: CMake скачает и соберёт SDL3 (несколько минут).
4. Выбрать цель `ladoga_01_window` и нажать Run.

Ожидаемо: окно 800×600 с заголовком `LadogaUI - Stage 1`, тёмный фон и синий
прямоугольник 400×300 по центру. Окно закрывается крестиком, процесс
завершается с кодом 0.

## 3. Сборка в терминале

### Windows: инструменты из CLion

```powershell
$clion = "C:\Programming\Apps\JetBrains\CLion 2026.2.4.1"
$env:PATH = "$clion\bin\cmake\win\x64\bin;$clion\bin\ninja\win\x64;$clion\bin\mingw\bin;$env:PATH"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\ladoga_01_window.exe
```

Все бинарники лежат в `<build>/bin`. Рядом с exe автоматически копируется
`libwinpthread-1.dll`; `libstdc++` и `libgcc` слинкованы статически, поэтому
запускать можно без CLion.

### Linux / macOS

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/bin/ladoga_01_window
```

Если Ninja не установлен, убрать `-G Ninja`: CMake выберет доступный генератор.
На Linux могут понадобиться пакеты разработки OpenGL и X11/Wayland — SDL3
собирается из исходников и сам сообщит, чего не хватает.

## 4. Минимальная программа

```cpp
#include <ladoga/ladoga.h>

class DemoWindow final : public ladoga::Window {
public:
    DemoWindow() : Window("LadogaUI - Stage 1", 800, 600) {}

    void paint(ladoga::Renderer& renderer) override {
        renderer.draw_rect({200, 150, 400, 300}, {74, 144, 226, 255});
    }
};

int main(int argc, char** argv) {
    ladoga::Application app(argc, argv);
    DemoWindow window;
    return app.run(window);
}
```

`Application` создаёт окно и цикл событий, `Window::paint` рисует содержимое,
`Renderer` скрывает OpenGL. Полный код — в
[examples/01_window/main.cpp](../examples/01_window/main.cpp).

## 5. Частые проблемы

| Симптом | Причина | Решение |
|---|---|---|
| CMake долго «висит» на `FetchContent` | Первая загрузка SDL3 | Дать сети время; проверить доступ к GitHub |
| Ошибка `SDL3` при конфигурации | Нет сети или системного SDL3 | Включить `-DLADOGA_USE_SYSTEM_SDL3=ON` или проверить сеть |
| `cmake: command not found` | CMake не в `PATH` | Открыть проект в CLion или добавить CMake в `PATH` |
| `CMAKE_CXX_COMPILER not found` | Нет компилятора в `PATH` | Использовать toolchain CLion или установить GCC/Clang/MSVC |
| Окно не открывается, код 1 | Нет OpenGL-драйвера / контекста | Обновить драйвер видеокарты, проверить вывод `stderr` |
| `0xC0000135` при запуске | Не хватает DLL | Запускать из `<build>/bin`, где лежит `libwinpthread-1.dll` |

## 6. Что дальше

1. Прочитать [контракт API](API_CONTRACT.md).
2. Прочитать [справочник по XML](XML_REFERENCE.md).
3. Открыть [план](PLAN.md) и начать с шага 2.1 — `Margin` и `Widget`.

Цель этапа 2: дерево виджетов и `StackPanel`, который раскладывает детей
вертикально и горизонтально.
