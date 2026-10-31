#include "xml/xml_loader.h"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <ladoga/slider.h>
#include <ladoga/widget.h>
#include <ladoga/window.h>

namespace ladoga::xml {

namespace {

std::pair<int, int> line_and_column(const std::string& source, std::ptrdiff_t offset) {
    const std::size_t limit =
        std::min(source.size(), static_cast<std::size_t>(std::max<std::ptrdiff_t>(offset, 0)));
    int line = 1;
    int column = 1;
    for (std::size_t i = 0; i < limit; ++i) {
        if (source[i] == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
    }
    return {line, column};
}

std::string attribute_name(const pugi::xml_attribute& attribute) {
    return std::string(attribute.name());
}

int parse_int(std::string_view text) {
    int value = 0;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) {
        throw std::invalid_argument("ожидалось целое число");
    }
    return value;
}

} // namespace

void XmlLoader::load(Window& window, const std::filesystem::path& path) {
    path_ = path.string();
    names_.clear();

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error(path_ + ": не удалось открыть файл");
    }
    source_ = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    pugi::xml_document document;
    const pugi::xml_parse_result result = document.load_buffer(source_.data(), source_.size());
    if (!result) {
        const auto [line, column] = line_and_column(source_, result.offset);
        throw std::runtime_error(path_ + ":" + std::to_string(line) + ":" +
                                 std::to_string(column) + ": " + result.description());
    }

    const pugi::xml_node root = document.document_element();
    if (!root) {
        throw std::runtime_error(path_ + ": пустой XML-документ");
    }
    if (std::string_view(root.name()) != "Window") {
        const auto [line, column] = line_and_column(source_, root.offset_debug());
        throw std::runtime_error(path_ + ":" + std::to_string(line) + ":" +
                                 std::to_string(column) +
                                 ": корневой тег должен быть <Window>");
    }

    apply_attributes(window, window, root);
    if (!window.name().empty()) {
        names_.insert(window.name());
    }
    load_children(window, root, window);
}

void XmlLoader::load_children(Window& window, const pugi::xml_node& parent, Widget& target) {
    for (const pugi::xml_node& node : parent.children()) {
        if (node.type() != pugi::node_element) {
            continue;
        }

        std::unique_ptr<Widget> widget;
        try {
            widget = factory_.create(node.name());
        } catch (const std::exception& error) {
            const auto [line, column] = line_and_column(source_, node.offset_debug());
            throw std::runtime_error(path_ + ":" + std::to_string(line) + ":" +
                                     std::to_string(column) + ": " + error.what());
        }

        apply_attributes(window, *widget, node);

        if (!widget->name().empty() && !names_.insert(widget->name()).second) {
            const auto [line, column] = line_and_column(source_, node.offset_debug());
            throw std::runtime_error(path_ + ":" + std::to_string(line) + ":" +
                                     std::to_string(column) + ": Name '" + widget->name() +
                                     "' уже используется");
        }

        load_children(window, node, *widget);
        target.add_child(std::move(widget));
    }
}

void XmlLoader::apply_attributes(Window& window, Widget& widget, const pugi::xml_node& node) {
    for (const pugi::xml_attribute& attribute : node.attributes()) {
        if (!properties_.contains(attribute.name())) {
            const auto [line, column] = line_and_column(source_, node.offset_debug());
            throw std::runtime_error(path_ + ":" + std::to_string(line) + ":" +
                                     std::to_string(column) + ": неизвестный атрибут '" +
                                     attribute_name(attribute) + "'");
        }
    }

    // У Slider Min и Max применяем вместе, иначе порядок атрибутов ломает диапазон.
    auto* slider = dynamic_cast<Slider*>(&widget);
    if (slider != nullptr) {
        const pugi::xml_attribute min_attribute = node.attribute("Min");
        const pugi::xml_attribute max_attribute = node.attribute("Max");
        if (min_attribute || max_attribute) {
            try {
                const int min = min_attribute ? parse_int(min_attribute.value()) : slider->min();
                const int max = max_attribute ? parse_int(max_attribute.value()) : slider->max();
                slider->set_range(min, max);
            } catch (const std::exception& error) {
                const auto [line, column] = line_and_column(source_, node.offset_debug());
                throw std::runtime_error(path_ + ":" + std::to_string(line) + ":" +
                                         std::to_string(column) + ": Min/Max: " + error.what());
            }
        }
    }

    // Порядок важен: Value должен примениться после Min/Max.
    static constexpr const char* order[] = {
        "Title", "Name", "Width", "Height", "Margin", "Visible",
        "Orientation", "Text", "Checked", "Click", "Min", "Max", "Value",
    };

    for (const char* name : order) {
        const bool slider_range = slider != nullptr &&
                                  (std::string_view(name) == "Min" ||
                                   std::string_view(name) == "Max");
        if (slider_range) {
            continue;
        }
        const pugi::xml_attribute attribute = node.attribute(name);
        if (attribute) {
            apply_attribute(window, widget, attribute, node.offset_debug());
        }
    }
}

void XmlLoader::apply_attribute(Window& window, Widget& widget,
                                const pugi::xml_attribute& attribute, std::ptrdiff_t offset) {
    try {
        properties_.apply(widget, attribute.name(), attribute.value(), window);
    } catch (const std::exception& error) {
        const auto [line, column] = line_and_column(source_, offset);
        throw std::runtime_error(path_ + ":" + std::to_string(line) + ":" +
                                 std::to_string(column) + ": свойство " +
                                 attribute_name(attribute) + ": " + error.what());
    }
}

} // namespace ladoga::xml
