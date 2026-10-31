# Быстрый старт: первое окно за 30 минут

Цель: собрать LadogaUI, запустить форму из XML, изменить текст и подключить
C++-обработчик кнопки. Без Qt, без ручной установки SDL3.

## 0–5 минут. Что нужно

- Windows, Linux или macOS;
- CMake 3.24+;
- компилятор C++20: MSVC 2022+, GCC 12+ или Clang 15+;
- OpenGL-драйвер;
- интернет при первой сборке: CMake скачивает SDL3 3.4.18 и pugixml 1.16.

## 5–12 минут. Сборка

### CLion

1. Открыть папку `LadogaUI`.
2. `File → Reload CMake Project`.
3. Дождаться, пока CMake скачает и соберёт SDL3 и pugixml.
4. Выбрать цель `ladoga_05_hello` и нажать Run.

### Терминал Windows

```powershell
$clion = "C:\Programming\Apps\JetBrains\CLion 2026.2.4.1"
$env:PATH = "$clion\bin\cmake\win\x64\bin;$clion\bin\ninja\win\x64;$clion\bin\mingw\bin;$env:PATH"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

### Терминал Linux / macOS

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## 12–15 минут. Первый запуск

```powershell
.\build\bin\ladoga_05_hello.exe
```

На Linux/macOS:

```bash
./build/bin/ladoga_05_hello
```

Ожидаемо: окно 560×440 с заголовком `LadogaUI - Hello, World`, текст
«Введите имя и нажмите OK», поле ввода, кнопка OK, флажок и слайдер.
Введите имя, нажмите OK — текст приветствия изменится. Консольного окна при
запуске нет; при ошибке открывается диалог с текстом ошибки.

![Форма Hello, World](images/hello.png)

## 15–22 минуты. Меняем XML

Откройте `examples/05_hello/main.xml`:

```xml
<Window Name="HelloWindow" Title="LadogaUI - Hello, World" Width="560" Height="440">
    <StackPanel Orientation="Vertical" Margin="24">
        <Label Name="greeting" Text="Введите имя и нажмите OK" />
        <TextBox Name="nameBox" Text="" />
        <Button Name="okButton" Text="OK" Click="onOk" />
        <CheckBox Name="agree" Text="Согласен" Checked="true" />
        <Slider Name="volume" Min="0" Max="100" Value="50" />
    </StackPanel>
</Window>
```

Измените `Text`, `Width`, `Title` и пересоберите — окно изменится. Незнакомые
теги и атрибуты дают ошибку с номером строки, а не молча игнорируются.

## 22–28 минут. Подключаем C++

В `examples/05_hello/main.cpp` обработчик уже есть:

```cpp
class HelloWindow final : public ladoga::Window {
public:
    HelloWindow() {
        load_from_xml(std::filesystem::path(LADOGA_EXAMPLE_DIR) / "main.xml");
        register_action("onOk", [this] { onOk(); });
    }

    ladoga::Label& greeting() { return *find_child<ladoga::Label>("greeting"); }
    ladoga::TextBox& nameBox() { return *find_child<ladoga::TextBox>("nameBox"); }

private:
    void onOk() {
        const std::string name = nameBox().text();
        greeting().set_text(name.empty() ? "Привет, мир!" : "Привет, " + name + "!");
    }
};
```

Поменяйте текст приветствия, добавьте ещё один `<Label>` или кнопку. Для
нового `Name` делается такой же короткий accessor через `find_child<T>`.

## 28–30 минут. Куда дальше

- [Практикум: подключение фреймворка к своему проекту и форма «Сумма двух чисел»](TUTORIAL_SUM.md)
- [Справочник API](API_REFERENCE.md)
- [Справочник XML](XML_REFERENCE.md)
- [Примеры](EXAMPLES.md)
- [Виджеты](widgets/README.md)
- [Частые проблемы](TROUBLESHOOTING.md)

Если что-то не запускается, начните с [TROUBLESHOOTING.md](TROUBLESHOOTING.md).
