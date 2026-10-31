# Button

Кликабельная кнопка. Аналог `QPushButton` в Qt и `Button` в WPF.

```xml
<Button Name="okButton" Text="OK" Click="onOk" />
```

## Свойства

| XML | C++ | Тип | По умолчанию |
|---|---|---|---|
| `Name` | `set_name` / `name` | string | пусто |
| `Text` | `set_text` / `text` | string | пусто |
| `Width` / `Height` | `set_width` / `set_height` | int / `Auto` | `Auto` |
| `Margin` | `set_margin` | int / 4 int | 0 |
| `Click` | `std::function<void()> clicked` | имя действия | пусто |

## Событие clicked

```cpp
okButton().clicked = [this] { onOk(); };
```

`clicked` вызывается только если нажатие и отпускание мыши произошли внутри
кнопки. После `Click="onOk"` в XML достаточно зарегистрировать действие:

```cpp
register_action("onOk", [this] { onOk(); });
```

Прямое назначение `clicked` перекрывает XML-привязку.

## C++

```cpp
auto button = std::make_unique<ladoga::Button>();
button->set_text("Нажми меня");
button->clicked = [] { /* ... */ };
panel->add_child(std::move(button));
```
