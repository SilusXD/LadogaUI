# CheckBox

Флажок вкл/выкл. Аналог `QCheckBox` в Qt и `CheckBox` в WPF.

```xml
<CheckBox Name="agree" Text="Согласен" Checked="true" />
```

## Свойства

| XML | C++ | Тип | По умолчанию |
|---|---|---|---|
| `Name` | `set_name` / `name` | string | пусто |
| `Text` | `set_text` / `text` | string | пусто |
| `Checked` | `set_checked` / `checked` | bool | `false` |
| `Width` / `Height` | `set_width` / `set_height` | int / `Auto` | `Auto` |
| `Margin` | `set_margin` | int / 4 int | 0 |

## Событие checked_changed

```cpp
agree().checked_changed = [this](bool checked) {
    // ...
};
```

Пользовательский клик переключает флажок и вызывает событие. Программный
`set_checked()` событие не вызывает.

## C++

```cpp
auto box = std::make_unique<ladoga::CheckBox>();
box->set_text("Согласен");
box->set_checked(true);
panel->add_child(std::move(box));
```
