#include "xml/property_map.h"

#include <charconv>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <ladoga/button.h>
#include <ladoga/check_box.h>
#include <ladoga/label.h>
#include <ladoga/slider.h>
#include <ladoga/stack_panel.h>
#include <ladoga/text_box.h>
#include <ladoga/widget.h>
#include <ladoga/window.h>

namespace ladoga::xml {

namespace {

int parse_int(std::string_view text) {
    int value = 0;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) {
        throw std::invalid_argument("ожидалось целое число");
    }
    return value;
}

bool parse_bool(std::string_view text) {
    if (text == "true") {
        return true;
    }
    if (text == "false") {
        return false;
    }
    throw std::invalid_argument("ожидалось true или false");
}

int parse_length(std::string_view text) {
    if (text == "Auto") {
        return Widget::Auto;
    }
    const int value = parse_int(text);
    if (value < 0) {
        throw std::invalid_argument("размер не может быть отрицательным");
    }
    return value;
}

Margin parse_margin(std::string_view text) {
    std::istringstream stream{std::string(text)};
    std::vector<int> values;
    int value = 0;
    while (stream >> value) {
        values.push_back(value);
    }
    if (!stream.eof()) {
        throw std::invalid_argument("ожидалось 1 или 4 целых числа");
    }
    if (values.size() == 1) {
        if (values[0] < 0) {
            throw std::invalid_argument("Margin не может быть отрицательным");
        }
        return Margin::all(values[0]);
    }
    if (values.size() == 4) {
        for (int item : values) {
            if (item < 0) {
                throw std::invalid_argument("Margin не может быть отрицательным");
            }
        }
        return {values[0], values[1], values[2], values[3]};
    }
    throw std::invalid_argument("ожидалось 1 или 4 целых числа");
}

std::string as_string(std::string_view value) { return std::string(value); }

void set_text(Widget& widget, std::string value) {
    if (auto* label = dynamic_cast<Label*>(&widget)) {
        label->set_text(std::move(value));
    } else if (auto* button = dynamic_cast<Button*>(&widget)) {
        button->set_text(std::move(value));
    } else if (auto* text_box = dynamic_cast<TextBox*>(&widget)) {
        text_box->set_text(std::move(value));
    } else if (auto* check_box = dynamic_cast<CheckBox*>(&widget)) {
        check_box->set_text(std::move(value));
    } else {
        throw std::invalid_argument("свойство Text не поддерживается этим виджетом");
    }
}

} // namespace

PropertyMap::PropertyMap() {
    add("Name", [](Widget& widget, std::string_view value, Window&) {
        widget.set_name(as_string(value));
    });
    add("Width", [](Widget& widget, std::string_view value, Window&) {
        widget.set_width(parse_length(value));
    });
    add("Height", [](Widget& widget, std::string_view value, Window&) {
        widget.set_height(parse_length(value));
    });
    add("Margin", [](Widget& widget, std::string_view value, Window&) {
        widget.set_margin(parse_margin(value));
    });
    add("Visible", [](Widget& widget, std::string_view value, Window&) {
        widget.set_visible(parse_bool(value));
    });
    add("Text", [](Widget& widget, std::string_view value, Window&) {
        set_text(widget, as_string(value));
    });
    add("Orientation", [](Widget& widget, std::string_view value, Window&) {
        auto* panel = dynamic_cast<StackPanel*>(&widget);
        if (panel == nullptr) {
            throw std::invalid_argument("Orientation допустим только у StackPanel");
        }
        if (value == "Vertical") {
            panel->set_orientation(Orientation::Vertical);
        } else if (value == "Horizontal") {
            panel->set_orientation(Orientation::Horizontal);
        } else {
            throw std::invalid_argument("Orientation: ожидалось Vertical или Horizontal");
        }
    });
    add("Checked", [](Widget& widget, std::string_view value, Window&) {
        auto* check_box = dynamic_cast<CheckBox*>(&widget);
        if (check_box == nullptr) {
            throw std::invalid_argument("Checked допустим только у CheckBox");
        }
        check_box->set_checked(parse_bool(value));
    });
    add("Min", [](Widget& widget, std::string_view value, Window&) {
        auto* slider = dynamic_cast<Slider*>(&widget);
        if (slider == nullptr) {
            throw std::invalid_argument("Min допустим только у Slider");
        }
        slider->set_min(parse_int(value));
    });
    add("Max", [](Widget& widget, std::string_view value, Window&) {
        auto* slider = dynamic_cast<Slider*>(&widget);
        if (slider == nullptr) {
            throw std::invalid_argument("Max допустим только у Slider");
        }
        slider->set_max(parse_int(value));
    });
    add("Value", [](Widget& widget, std::string_view value, Window&) {
        auto* slider = dynamic_cast<Slider*>(&widget);
        if (slider == nullptr) {
            throw std::invalid_argument("Value допустим только у Slider");
        }
        slider->set_value(parse_int(value));
    });
    add("Title", [](Widget& widget, std::string_view value, Window&) {
        auto* window = dynamic_cast<Window*>(&widget);
        if (window == nullptr) {
            throw std::invalid_argument("Title допустим только у Window");
        }
        window->set_title(as_string(value));
    });
    add("Click", [](Widget& widget, std::string_view value, Window& window) {
        auto* button = dynamic_cast<Button*>(&widget);
        if (button == nullptr) {
            throw std::invalid_argument("Click допустим только у Button");
        }
        const std::string action(value);
        button->clicked = [&window, action] { window.invoke_action(action); };
    });
}

bool PropertyMap::contains(std::string_view name) const {
    return setters_.find(name) != setters_.end();
}

void PropertyMap::apply(Widget& widget, std::string_view name, std::string_view value,
                        Window& window) const {
    const auto it = setters_.find(name);
    if (it == setters_.end()) {
        throw std::invalid_argument("неизвестное свойство");
    }
    it->second(widget, value, window);
}

void PropertyMap::add(std::string name, Setter setter) {
    setters_[std::move(name)] = std::move(setter);
}

} // namespace ladoga::xml
