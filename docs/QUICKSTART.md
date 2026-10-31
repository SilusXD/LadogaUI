# Быстрый старт

Статус: **этапы 2–3.** Работают дерево виджетов, `StackPanel` и загрузка окна
из XML. Текст пока не рисуется: `Label`, `Button`, `TextBox`, `CheckBox` и
`Slider` выглядят как цветные заглушки. Шрифты — этап 5, клики — этап 4.

## 1. Что нужно

- Windows, Linux или macOS;
- CMake 3.24+;
- компилятор C++20: MSVC 2022+, GCC 12+ или Clang 15+;
- OpenGL-драйвер;
- интернет при первой сборке: CMake скачивает SDL3 3.4.18 и pugixml 1.16.

Если зависимости уже стоят в системе, передайте
`-DLADOGA_USE_SYSTEM_SDL3=ON` и `-DLADOGA_USE_SYSTEM_PUGIXML=ON`.

## 2. Сборка в CLion

1. Открыть папку `LadogaUI`.
2. `File → Reload CMake Project`.
3. Дождаться первой настройки: CMake скачает и соберёт SDL3 и pugixml.
4. Выбрать цель `ladoga_03_xml` и нажать Run.

Ожидаемо: окно 520×420 с заголовком `LadogaUI - XML`, внутри — цветные
заглушки пяти виджетов. Окно закрывается крестиком, процесс завершается с кодом 0.

## 3. Сборка в терминале

### Windows: инструменты из CLion

```powershell
$clion = "C:\Programming\Apps\JetBrains\CLion 2026.2.4.1"
$env:PATH = "$clion\bin\cmake\win\x64\bin;$clion\bin\ninja\win\x64;$clion\bin\mingw\bin;$env:PATH"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bin\ladoga_03_xml.exe
```

Все бинарники лежат в `<build>/bin`. Рядом с exe автоматически копируются
`libwinpthread-1.dll` и `main.xml`; C++ runtime слинкован статически.

### Linux / macOS

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/bin/ladoga_03_xml
```

Если Ninja не установлен, убрать `-G Ninja`: CMake выберет доступный генератор.
Для сборки SDL3 на Linux могут понадобиться пакеты разработки OpenGL и X11/Wayland.

## 4. Минимальный XML

Файл `main.xml`:

```xml
<Window Name="MainWindow" Title="Hello" Width="400" Height="300">
    <StackPanel Orientation="Vertical" Margin="16">
        <Label Name="greeting" Text="Введите имя:" />
        <TextBox Name="nameBox" Text="" />
        <Button Name="okButton" Text="OK" Click="onOk" />
    </StackPanel>
</Window>
```

Загрузка и доступ по имени:

```cpp
#include <ladoga/ladoga.h>

class MainWindow final : public ladoga::Window {
public:
    MainWindow() {
        load_from_xml("main.xml");
        register_action("onOk", [this] { onOk(); });
    }

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

`load_from_xml("main.xml")` ищет файл относительно рабочего каталога. В готовом
примере `03_xml` путь передаётся абсолютным, чтобы запуск из CLion и из
`build/bin` работал одинаково.

## 5. Частые проблемы

| Симптом | Причина | Решение |
|---|---|---|
| CMake долго «висит» на `FetchContent` | Первая загрузка SDL3/pugixml | Дать сети время; проверить доступ к GitHub |
| `main.xml:3:5: неизвестный тег <Buton>` | Опечатка в теге | Тег должен совпадать с классом: `Button` |
| `свойство Text: ...` | `Text` у виджета, которому оно не положено | `Text` есть только у Label/Button/TextBox/CheckBox |
| `Name 'x' уже используется` | Два виджета с одним `Name` | Имена в одном окне уникальны |
| Окно не открывается, код 1 | Нет OpenGL-драйвера | Обновить драйвер, посмотреть `stderr` |
| `0xC0000135` при запуске | Не хватает DLL | Запускать из `<build>/bin`, где лежит `libwinpthread-1.dll` |

## 6. Что дальше

1. [Контракт API](API_CONTRACT.md).
2. [Справочник по XML](XML_REFERENCE.md).
3. [План](PLAN.md), этап 4 — hit-test, клики и `Button::clicked`.

Цель этапа 4: клик по кнопке действительно вызывает `onOk`, а `clicked`
вызывается только при нажатии и отпускании внутри кнопки.
