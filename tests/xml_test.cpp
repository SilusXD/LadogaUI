#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <ladoga/ladoga.h>

namespace {

int failures = 0;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

std::filesystem::path write_xml(const std::string& name, const std::string& content) {
    const std::filesystem::path path = std::filesystem::current_path() / name;
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("cannot write " + path.string());
    }
    file << content;
    file.close();
    return path;
}

} // namespace

int main() {
    const std::filesystem::path good =
        write_xml("ladoga_xml_test_good.xml",
                  R"(<Window Name="Main" Title="Test" Width="400" Height="300">
    <StackPanel Name="root" Orientation="Vertical" Margin="10">
        <Label Name="greeting" Text="Hello" />
        <Button Name="ok" Text="OK" Click="onOk" />
        <CheckBox Name="agree" Text="Yes" Checked="true" />
        <Slider Name="volume" Min="0" Max="100" Value="50" />
        <Slider Name="wide" Min="200" Max="300" Value="250" />
    </StackPanel>
</Window>)");

    try {
        ladoga::Window window;
        window.load_from_xml(good);

        check(window.title() == "Test", "root Title");
        check(window.width() == 400, "root Width");
        check(window.height() == 300, "root Height");

        auto* panel = window.find_child<ladoga::StackPanel>("root");
        check(panel != nullptr, "StackPanel found");
        if (panel != nullptr) {
            check(panel->orientation() == ladoga::Orientation::Vertical, "Orientation");
        }

        auto* label = window.find_child<ladoga::Label>("greeting");
        check(label != nullptr, "Label found");
        if (label != nullptr) {
            check(label->text() == "Hello", "Label Text");
        }

        auto* button = window.find_child<ladoga::Button>("ok");
        check(button != nullptr, "Button found");
        if (button != nullptr) {
            check(button->text() == "OK", "Button Text");
            bool action_called = false;
            window.register_action("onOk", [&action_called] { action_called = true; });
            check(static_cast<bool>(button->clicked), "Click attached");
            button->clicked();
            check(action_called, "Click invokes action");
        }

        auto* check_box = window.find_child<ladoga::CheckBox>("agree");
        check(check_box != nullptr, "CheckBox found");
        if (check_box != nullptr) {
            check(check_box->checked(), "CheckBox Checked");
        }

        auto* slider = window.find_child<ladoga::Slider>("volume");
        check(slider != nullptr, "Slider found");
        if (slider != nullptr) {
            check(slider->value() == 50, "Slider Value");
        }

        auto* wide_slider = window.find_child<ladoga::Slider>("wide");
        check(wide_slider != nullptr, "Wide Slider found");
        if (wide_slider != nullptr) {
            check(wide_slider->min() == 200 && wide_slider->max() == 300,
                  "Slider Min/Max order independent");
            check(wide_slider->value() == 250, "Wide Slider Value");
        }

        window.set_bounds({0, 0, 400, 300});
        window.layout();
        if (label != nullptr) {
            check(label->bounds().x == 10 && label->bounds().y == 10, "Label layout from XML");
        }
    } catch (const std::exception& error) {
        std::cerr << "unexpected XML error: " << error.what() << '\n';
        ++failures;
    }

    const std::filesystem::path bad =
        write_xml("ladoga_xml_test_bad.xml", R"(<Window><Buton /></Window>)");
    try {
        ladoga::Window window;
        window.load_from_xml(bad);
        check(false, "unknown tag should throw");
    } catch (const std::runtime_error& error) {
        check(std::string(error.what()).find("Buton") != std::string::npos,
              "unknown tag error mentions tag");
    }

    const std::filesystem::path bad_range =
        write_xml("ladoga_xml_test_bad_range.xml",
                  R"(<Window><Slider Min="10" Max="5" /></Window>)");
    try {
        ladoga::Window window;
        window.load_from_xml(bad_range);
        check(false, "Min > Max should throw");
    } catch (const std::runtime_error& error) {
        check(std::string(error.what()).find("Min/Max") != std::string::npos,
              "Min > Max error mentions range");
    }

    std::filesystem::remove(good);
    std::filesystem::remove(bad);
    std::filesystem::remove(bad_range);

    if (failures == 0) {
        std::cout << "xml test: OK\n";
        return 0;
    }
    std::cerr << "xml test: " << failures << " failure(s)\n";
    return 1;
}
