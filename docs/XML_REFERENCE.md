# Справочник по XML-разметке

Статус: **контракт этапа 0**. Парсер появится на этапе 3; до этого документ
фиксирует, что именно он должен принимать. Любое изменение синтаксиса — это
изменение контракта.

## 1. Минимальный пример

```xml
<Window Name="MainWindow" Title="Hello" Width="400" Height="300">
    <StackPanel Orientation="Vertical" Margin="16">
        <Label Name="greeting" Text="Введите имя:" />
        <TextBox Name="nameBox" Text="" />
        <Button Name="okButton" Text="OK" Click="onOk" />
        <CheckBox Name="agree" Text="Согласен" Checked="true" />
        <Slider Name="volume" Min="0" Max="100" Value="50" />
    </StackPanel>
</Window>
```

Соответствие:

- тег = C++-класс (`Window`, `StackPanel`, `Label`, `Button`, `TextBox`,
  `CheckBox`, `Slider`);
- атрибут = свойство (`Text`, `Width`, `Orientation`, ...);
- вложенность = дерево детей;
- `Click="onOk"` = имя действия, зарегистрированного в окне.

## 2. Общие правила

1. Кодировка — UTF-8.
2. Один корневой элемент, и это `<Window>`.
3. Имена тегов и атрибутов чувствительны к регистру: `<Button>`, а не `<button>`.
4. Значения атрибутов всегда в двойных кавычках.
5. Текстовое содержимое тегов не используется: весь текст передаётся через
   атрибут `Text`. Пробелы между тегами игнорируются.
6. Неизвестный тег или атрибут — ошибка загрузки, а не молчаливое игнорирование.
7. `Name` должен быть уникальным в пределах окна. Повтор — ошибка.
8. `Name` не обязателен, но без него нельзя найти виджет через `find_child`.
9. Ссылки на действия (`Click`) не проверяются при загрузке: действие может
   быть зарегистрировано позже. Отсутствие действия обнаруживается при клике.
10. Ошибка сообщает файл, строку и колонку:
    `main.xml:12:5: неизвестный тег <Buton>`.

## 3. Корневой элемент Window

```xml
<Window Name="MainWindow"
        Title="Hello"
        Width="400"
        Height="300">
    <!-- дети -->
</Window>
```

| Атрибут | Тип | По умолчанию | Смысл |
|---|---|---|---|
| Name | string | пусто | имя окна |
| Title | string | `LadogaUI` | заголовок нативного окна |
| Width | int / `Auto` | 800 | ширина окна |
| Height | int / `Auto` | 600 | высота окна |
| Margin | margin | 0 | отступы корневого контейнера |
| Visible | bool | `true` | видимость окна |

`load_from_xml` применяет атрибуты корня к уже существующему объекту `Window`,
а не создаёт второе окно. Дети корня становятся детьми этого окна.

## 4. Общие атрибуты виджетов

| Атрибут | Тип | По умолчанию | Где допустим |
|---|---|---|---|
| Name | string | пусто | все |
| Width | int / `Auto` | `Auto` | все |
| Height | int / `Auto` | `Auto` | все |
| Margin | int или 4 int | 0 | все |
| Visible | bool | `true` | все |
| Text | string | пусто | Label, Button, TextBox, CheckBox |
| Orientation | `Vertical` / `Horizontal` | `Vertical` | StackPanel |
| Checked | bool | `false` | CheckBox |
| Value | int | 0 | Slider |
| Min | int | 0 | Slider |
| Max | int | 100 | Slider |
| Click | string | пусто | Button |

### Числа

- Только целые числа: `Width="400"`, `Margin="16"`, `Value="50"`.
- Единицы измерения не пишутся.
- `Width`/`Height`: либо неотрицательное целое, либо строка `Auto`.
- `Visible`/`Checked`: только `true` или `false` (нижний регистр).
- `Value` зажимается в диапазон `[Min, Max]` при загрузке.
- Если `Min > Max`, это ошибка загрузки.

### Margin

Один int — все стороны:

```xml
<Label Margin="16" />
```

Четыре int через пробел — `left top right bottom`:

```xml
<Label Margin="16 8 16 8" />
```

Отрицательные значения запрещены.

## 5. Теги

### Window

Корень. Создаёт/настраивает окно. Дети — любые виджеты, обычно один StackPanel.

### StackPanel

```xml
<StackPanel Name="root" Orientation="Vertical" Margin="16">
    <Label Text="Сверху" />
    <Label Text="Снизу" />
</StackPanel>
```

`Orientation="Vertical"` — дети сверху вниз (по умолчанию).
`Orientation="Horizontal"` — дети слева направо.

`Width`/`Height` панели могут быть `Auto`: тогда размер считается по детям.
`Margin` ребёнка учитывается при раскладке.

### Label

```xml
<Label Name="greeting" Text="Введите имя:" />
```

Текст только для чтения. Позже появятся выравнивание и перенос строк.

### Button

```xml
<Button Name="okButton" Text="OK" Click="onOk" />
```

`Click` — имя действия. Оно связывается с C++ так:

```cpp
window.register_action("onOk", [this] { onOk(); });
```

`clicked` срабатывает по отпусканию мыши внутри кнопки, если нажатие было
внутри. Прямое назначение `okButton().clicked` перекрывает XML-привязку.

### TextBox

```xml
<TextBox Name="nameBox" Text="" />
```

Однострочный ввод. `Text` — начальное значение. Пользовательское изменение
вызывает `text_changed`; программный `set_text` — нет.

### CheckBox

```xml
<CheckBox Name="agree" Text="Согласен" Checked="true" />
```

`Checked` — начальное состояние. Пользовательское переключение вызывает
`checked_changed`.

### Slider

```xml
<Slider Name="volume" Min="0" Max="100" Value="50" />
```

`Value` зажимается в `[Min, Max]`. Пользовательское движение вызывает
`value_changed`. `Min`/`Max` можно менять и из C++.

## 6. Что пока нельзя

- `GridPanel` и атрибуты строк/столбцов;
- стили, цвета, шрифты, темы;
- data binding и выражения в атрибутах;
- обработчики, кроме `Click`;
- несколько корневых окон в одном файле;
- ссылки на другие XML-файлы.

Эти возможности не «забыты»: они вынесены в отдельные шаги плана.

## 7. Пример для этапа 3 (ожидаемый результат)

Файл `examples/01_hello/main.xml`:

```xml
<Window Name="MainWindow" Title="Hello" Width="400" Height="300">
    <StackPanel Orientation="Vertical" Margin="16">
        <Label Text="Привет, мир!" />
        <Button Text="OK" Click="onOk" />
    </StackPanel>
</Window>
```

После этапа 3 окно должно открыться с текстом и кнопкой. После этапа 4 клик по
кнопке вызовет `onOk`. После этапа 5 появится полноценная форма с вводом имени.
