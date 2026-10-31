# Примеры

Все примеры собираются в `<build>/bin` и запускаются из любой папки.
`01_window`–`05_hello` — GUI-приложения без консольного окна; `00_smoke` и
тесты — консольные.

| Пример | Что показывает | Запуск |
|---|---|---|
| `01_window` | SDL3 + OpenGL, окно и прямоугольник | `ladoga_01_window.exe` |
| `02_layout` | `Widget`, `StackPanel`, вложенные прямоугольники | `ladoga_02_layout.exe` |
| `03_xml` | окно из XML, `Load_from_xml`, `Click="onOk"` | `ladoga_03_xml.exe` |
| `04_events` | клик, CheckBox, Slider и события | `ladoga_04_events.exe` |
| `05_hello` | итоговая форма «Hello, World» из XML | `ladoga_05_hello.exe` |

## 01_window

Минимальное окно 800×600 с синим прямоугольником. Показывает, как
`Application`, `Window` и `Renderer` работают вместе.

## 02_layout

`ColoredBox : public Widget` рисует себя, а `StackPanel` раскладывает детей
вертикально. Внутри — вторая панель с двумя прямоугольниками.

## 03_xml

Окно целиком описано в `main.xml`: `Window`, `StackPanel`, `Label`, `TextBox`,
`Button`, `CheckBox`, `Slider`. Нажатие кнопки вызывает `onOk` и меняет текст и
заголовок.

## 04_events

Виджеты создаются в C++: кнопка, флажок и слайдер меняют текст Label через
`clicked`, `checked_changed` и `value_changed`.

## 05_hello

Итоговая форма: введите имя, нажмите OK — приветствие меняется. Использует
XML, `TextBox`, `Button`, `CheckBox` и `Slider`.

## Сборка и запуск

```powershell
cmake --build build
.\build\bin\ladoga_05_hello.exe
```

В CLion выберите соответствующую цель (`ladoga_05_hello`) и нажмите Run.
