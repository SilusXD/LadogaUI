# Label

Текст только для чтения. Аналог `QLabel` в Qt и `Label` в WPF.

```xml
<Label Name="greeting" Text="Введите имя:" />
```

## Свойства

| XML | C++ | Тип | По умолчанию |
|---|---|---|---|
| `Name` | `set_name` / `name` | string | пусто |
| `Text` | `set_text` / `text` | string | пусто |
| `Width` / `Height` | `set_width` / `set_height` | int / `Auto` | `Auto` |
| `Margin` | `set_margin` | int / 4 int | 0 |
| `Visible` | `set_visible` | bool | `true` |

## Размер

Если `Width`/`Height` не заданы, размер считается по тексту и равен примерно
длине строки × 8 px, высота — 24 px. Текст рисуется шрифтом Roboto 16 px.

## C++

```cpp
auto label = std::make_unique<ladoga::Label>();
label->set_text("Привет");
panel->add_child(std::move(label));
```

## См. также

- [Button](button.md)
- [TextBox](text_box.md)
