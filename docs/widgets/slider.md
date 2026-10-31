# Slider

Числовой диапазон. Аналог `QSlider` в Qt и `Slider` в WPF.

```xml
<Slider Name="volume" Min="0" Max="100" Value="50" />
```

## Свойства

| XML | C++ | Тип | По умолчанию |
|---|---|---|---|
| `Name` | `set_name` / `name` | string | пусто |
| `Min` | `set_min` / `min` | int | 0 |
| `Max` | `set_max` / `max` | int | 100 |
| `Value` | `set_value` / `value` | int | 0 |
| `Width` / `Height` | `set_width` / `set_height` | int / `Auto` | `Auto` |
| `Margin` | `set_margin` | int / 4 int | 0 |

`Value` зажимается в `[Min, Max]`. Если `Min > Max`, XML-загрузка выдаёт ошибку.

## Событие value_changed

```cpp
volume().value_changed = [this](int value) {
    // ...
};
```

Событие вызывается при перетаскивании бегунка и клике по дорожке. Программный
`set_value()` событие не вызывает.

## C++

```cpp
auto slider = std::make_unique<ladoga::Slider>();
slider->set_range(0, 100);
slider->set_value(50);
panel->add_child(std::move(slider));
```
