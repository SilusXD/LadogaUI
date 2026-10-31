# PROJECT_BRIEF.md — Учебный C++ GUI-фреймворк

> Исходный бриф проекта. Хранится как источник истины: если код расходится с
> этим документом, сначала обсуждаем изменение контракта, потом меняем код.

## Итог проекта

Учебный C++ GUI-фреймворк с декларативной разметкой, который:
- По синтаксису и именам 1:1 узнаваем для тех, кто потом откроет Qt или WPF.
- Запускается на Windows, Linux, macOS и, в перспективе, на РЕД ОС.
- Имеет низкий порог входа: первый работающий пример — за 30 минут,
  без «взрослых» тем (сети, БД, многопоточность).
- Строится по принципу retained-mode (дерево виджетов хранит состояние),
  как Qt Widgets и WPF, а не immediate-mode, как Dear ImGui.
- Внутреннее устройство (парсер, рефлексия, маршрутизация событий) —
  вторично и скрыто от студента; публичный API и документация — первичны.

## Дорожная карта

| Этап | Цель | Ключевой результат | Критерий готовности |
|---|---|---|---|
| 0. Контракт API | Зафиксировать имена и XML-синтаксис | Этот документ | Студент читает пример и понимает, что произойдёт |
| 1. Окно и прямоугольник | SDL3 + OpenGL, абстрактный Renderer | Окно с цветным прямоугольником | Цикл событий работает, окно закрывается |
| 2. Дерево и макет | Widget, StackPanel, layout() | Два вложенных прямоугольника | StackPanel располагает детей вертикально/горизонтально |
| 3. XML → объекты | Парсер (pugixml) + фабрика + карта свойств | Окно, описанное в XML, рисуется | Text="OK" доходит до Label |
| 4. События и клик | Hit-test + std::function / сигнал | Кнопка меняет текст по клику | Click="onOk" вызывает C++-функцию |
| 5. Виджеты и текст | Button, Label, TextBox, CheckBox, Slider; шрифты | Форма «Hello, World» | 5 виджетов + справочник по каждому |
| 6. Документация | Quickstart, справочник XML, 3–5 примеров | Студент проходит курс без вашего участия | «30-минутный пример» реально работает |
| 7. Импортозамещение | Сборка и тест на РЕД ОС / Linux | Пакет / инструкция по сборке | Запускается на чистой РЕД ОС |
| 8. Коммерческий слой | Плагины, темы, кастомизация | Точки расширения для вас | Новый виджет добавляется без правки ядра |

**Правило этапов 5–6:** каждый новый виджет = новый пример + страница справки.
Документация пишется параллельно с кодом, а не после.

## Дизайн публичного API (сверка с Qt/WPF)

### Классы и наследование

Widget (базовый)
├── Window          (как QMainWindow / Window)
├── StackPanel      (как QVBoxLayout+QHBoxLayout / StackPanel)
├── GridPanel       (как QGridLayout / Grid — позже)
├── Label           (как QLabel / Label)
├── Button          (как QPushButton / Button)
├── TextBox         (как QLineEdit / TextBox)
├── CheckBox        (как QCheckBox / CheckBox)
└── Slider          (как QSlider / Slider)

| Наш класс | Qt-аналог | WPF-аналог | Назначение |
|---|---|---|---|
| Widget | QWidget | UIElement / FrameworkElement | База: bounds, parent, children, layout(), paint(), on_event() |
| Window | QMainWindow / QDialog | Window | Корневое окно, заголовок, размер |
| StackPanel | QVBoxLayout / QHBoxLayout | StackPanel | Линейная раскладка детей |
| GridPanel | QGridLayout | Grid | Раскладка по строкам/столбцам (этап 5+) |
| Label | QLabel | Label | Текст только для чтения |
| Button | QPushButton | Button | Кликабельная кнопка, сигнал clicked |
| TextBox | QLineEdit | TextBox | Ввод одной строки |
| CheckBox | QCheckBox | CheckBox | Флаг вкл/выкл |
| Slider | QSlider | Slider | Числовой диапазон |

### Свойства (общие и специфичные)

| Свойство | Тип | Где есть | Qt-аналог | WPF-аналог |
|---|---|---|---|---|
| Name | string | Widget | objectName | x:Name |
| Width / Height | int (px) или Auto | Widget | setFixedWidth/Height | Width/Height |
| Margin | int или 4 int | Widget | setContentsMargins | Margin |
| Visible | bool | Widget | setVisible | Visibility |
| Text | string | Label, Button, TextBox, CheckBox | setText / text | Content / Text |
| Orientation | Vertical / Horizontal | StackPanel | направление layout | Orientation |
| Checked | bool | CheckBox | isChecked | IsChecked |
| Value / Min / Max | int | Slider | value/minimum/maximum | Value/Minimum/Maximum |

### События

Для простоты и образовательной ценности используем std::function в публичном API,
но с возможностью позже заменить на сигналы/слоты.

```cpp
// Button
std::function<void()> clicked;      // Qt: signal clicked()
// TextBox
std::function<void(const std::string&)> text_changed;
// CheckBox
std::function<void(bool)> checked_changed;
```

Почему не сигналы/слоты сразу? Сигналы/слоты — мощная, но «взрослая» концепция.
std::function даёт студенту понятный on_click = [&]{ ... }, а внутри вы позже
сможете добавить сигнальную обёртку, не ломая API. Для желающих есть header-only
библиотеки сигналов/слотов, например sigslot.

### XML-синтаксис (диалект, близкий к XAML и Qt .ui)

Qt Designer хранит формы в XML (.ui), где теги — это классы, а свойства —
вложенные элементы. WPF XAML использует атрибуты и вложенность.
Наш синтаксис — смесь, максимально простая для чтения.

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

Правила:
- Тег = класс (Window, StackPanel, Button…).
- Атрибуты — свойства (Text, Width, Orientation…).
- Click="onOk" — имя C++-функции/метода, который вызовется.
- Вложенность = дерево детей (как в XAML и Qt .ui).

Связывание с C++:

```cpp
class MainWindow : public Window {
public:
    MainWindow() {
        load_from_xml("main.xml");     // создаёт дерево
        okButton().clicked = [this]{ onOk(); };
    }
    void onOk() { greeting().set_text("Привет, " + nameBox().text()); }
};
```

## Промпт для ИИ-агента

# Контекст проекта

Мы разрабатываем учебный C++ GUI-фреймворк с декларативной разметкой (XML) для студентов.
Первичная цель — показать, КАК ПОЛЬЗОВАТЬСЯ фреймворком. Внутреннее устройство — вторично и скрыто.
Фреймворк должен быть узнаваем: имена классов, методов и XML-тегов совпадают с Qt/WPF там, где это возможно.
Платформы: Windows, Linux, macOS; в перспективе — РЕД ОС.
Модель: retained-mode (дерево виджетов хранит состояние), как Qt Widgets и WPF, а не immediate-mode.

# Текущая задача

[ОПИШИТЕ ЗДЕСЬ КОНКРЕТНУЮ ЗАДАЧУ, например: «Реализуй парсер XML в дерево Widget с фабрикой по имени тега» или «Добавь виджет Slider с событиями value_changed»]

# Архитектурные правила

1. Публичный API — это учебник. Имена: Widget, Window, StackPanel, Label, Button, TextBox, CheckBox, Slider.
   Свойства: Name, Text, Width, Height, Margin, Orientation, Checked, Value, Min, Max.
   События: std::function, например std::function<void()> clicked.
2. XML-теги = имена классов. Атрибуты = свойства. Вложенность = дети.
   Пример: `<Button Name="ok" Text="OK" Click="onOk"/>`
3. Внутренняя реализация может быть прагматичной и наивной (std::map для фабрики, ручная карта свойств).
   Не используй тяжёлую рефлексию, макросы или метапрограммирование без крайней необходимости.
4. Бэкенд: SDL3 (окно/ввод) + OpenGL (рендеринг). Абстрагируй через интерфейсы WindowBackend и Renderer.
5. Не добавляй «взрослый» функционал: сети, базы данных, многопоточность, сложные анимации — пока.
6. Каждый новый виджет сопровождается: (а) реализацией, (б) XML-примером, (в) краткой справкой.

# Референсы (для вдохновения, не для копирования)

- XamlCpp — C++/XAML каркас с фабрикой и рефлексией: https://github.com/vanehu/XamlCpp
- Qt Designer .ui — формат XML-описания формы
- WPF Dependency Properties — свойства с уведомлением об изменении (на будущее)
- sigslot — header-only сигналы/слоты, если понадобятся
- SDL3 + OpenGL — минимальный бэкенд

# Формат ответа

- Сначала — краткий план (3–7 пунктов).
- Затем — код с комментариями на русском (только там, где неочевидно).
- В конце — как проверить, что работает (команда сборки / ожидаемый результат).
- Если решение временное или упрощённое — явно пометь это комментарием `// TODO: упрощение для этапа N`.

# Ограничения

- Не предлагай Qt, WPF, C# или проприетарные зависимости.
- Не усложняй API: студент должен увидеть `<Button Text="OK"/>` и понять, что произойдёт.
- При конфликте «красиво» vs «понятно студенту» — выбирай «понятно студенту».
