# Быстрый старт

Статус: **черновик этапа 0.** Пока фреймворк не рисует окно; этот файл
проверяет, что каркас собирается. Полноценный Quickstart с первым окном
появится после этапа 1 и будет переписан на этапе 6.

## 1. Что нужно

- CMake 3.24+;
- компилятор C++20;
- Ninja (или другой генератор CMake);
- Git.

SDL3, OpenGL и pugixml на этом этапе не нужны.

## 2. Сборка в CLion

1. Открыть папку `LadogaUI`.
2. `File → Reload CMake Project`.
3. Дождаться индексации.
4. В конфигурации запуска выбрать цель `ladoga_00_smoke`.
5. Run.

Ожидаемый вывод:

```text
LadogaUI 0.1.0
Frame: 400x300
Scaffold is ready. Next: SDL3 window and rectangle.
```

## 3. Сборка в терминале

### Windows: инструменты из CLion

```powershell
$clion = "C:\Programming\Apps\JetBrains\CLion 2026.2.4.1"
$env:PATH = "$clion\bin\cmake\win\x64\bin;$clion\bin\ninja\win\x64;$clion\bin\mingw\bin;$env:PATH"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\ladoga_00_smoke.exe
```

Если запуск из терминала падает с кодом `-1073741515` (в шестнадцатеричном виде
`0xC0000135`), это «DLL не найдена»: в `PATH` нет
`C:\Programming\Apps\JetBrains\CLion 2026.2.4.1\bin\mingw\bin` с `libstdc++-6.dll`.
Из CLion Run такой проблемы нет. Постоянное решение — отдельный шаг (см.
[план](PLAN.md), шаг 1.0).

Ожидаемо:

```text
...
smoke test: OK
...
LadogaUI 0.1.0
...
```

### Linux / macOS

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/bin/ladoga_00_smoke
```

Если Ninja не установлен, убрать `-G Ninja`: CMake выберет доступный генератор.

## 4. Частые проблемы этапа 0

| Симптом | Причина | Решение |
|---|---|---|
| `cmake: command not found` | CMake не в `PATH` | Открыть проект в CLion или добавить CMake в `PATH` |
| `CMAKE_CXX_COMPILER not found` | Нет компилятора в `PATH` | Использовать toolchain CLion или установить GCC/Clang/MSVC |
| `No such file: ladoga/ladoga.h` | Не подключён include-каталог | Собирать через CMake, а не вручную |
| Тест не найден | Не выполнен `include(CTest)` / неверная папка | Переконфигурировать `build/` с нуля |

## 5. Что дальше

1. Прочитать [контракт API](API_CONTRACT.md).
2. Прочитать [справочник по XML](XML_REFERENCE.md).
3. Открыть [план](PLAN.md) и начать с шага 1.1 — подключение SDL3.

Цель этапа 1: окно 800×600 с цветным прямоугольником, которое закрывается
крестиком.
