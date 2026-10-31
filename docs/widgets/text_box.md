# TextBox

Однострочный ввод. Аналог `QLineEdit` в Qt и `TextBox` в WPF.

```xml
<TextBox Name="nameBox" Text="" />
```

## Свойства

| XML | C++ | Тип | По умолчанию |
|---|---|---|---|
| `Name` | `set_name` / `name` | string | пусто |
| `Text` | `set_text` / `text` | string | пусто |
| `Width` / `Height` | `set_width` / `set_height` | int / `Auto` | `Auto` |
| `Margin` | `set_margin` | int / 4 int | 0 |

## Ввод

- клик по TextBox ставит фокус;
- печатный текст добавляется в конец строки;
- Backspace удаляет последний символ UTF-8;
- каретка рисуется сразу после текста с учётом реальной ширины шрифта;
- Enter пока не обрабатывается;
- программный `set_text()` не вызывает `text_changed`.

## Событие text_changed

```cpp
nameBox().text_changed = [this](const std::string& text) {
    // ...
};
```

Событие вызывается после пользовательского ввода, а не после `set_text`.

## C++

```cpp
auto box = std::make_unique<ladoga::TextBox>();
box->set_text("Иван");
panel->add_child(std::move(box));
```
