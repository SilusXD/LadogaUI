#pragma once

#include <cstddef>
#include <filesystem>
#include <set>
#include <string>

#include <pugixml.hpp>

#include "xml/property_map.h"
#include "xml/widget_factory.h"

namespace ladoga {
class Widget;
class Window;
} // namespace ladoga

namespace ladoga::xml {

/// Загрузчик XML в дерево виджетов. Внутренний класс, студент его не трогает.
class XmlLoader {
public:
    void load(Window& window, const std::filesystem::path& path);

private:
    void load_children(Window& window, const pugi::xml_node& parent, Widget& target);
    void apply_attributes(Window& window, Widget& widget, const pugi::xml_node& node);
    void apply_attribute(Window& window, Widget& widget, const pugi::xml_attribute& attribute,
                         std::ptrdiff_t offset);

    WidgetFactory factory_;
    PropertyMap properties_;
    std::set<std::string> names_;
    std::string source_;
    std::string path_;
};

} // namespace ladoga::xml
