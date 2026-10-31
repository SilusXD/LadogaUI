#pragma once

#include <functional>
#include <map>
#include <string>
#include <string_view>

namespace ladoga {
class Widget;
class Window;
} // namespace ladoga

namespace ladoga::xml {

/// Ручная карта "имя атрибута → сеттер". Без рефлексии и макросов.
class PropertyMap {
public:
    using Setter = std::function<void(Widget&, std::string_view, Window&)>;

    PropertyMap();

    [[nodiscard]] bool contains(std::string_view name) const;
    void apply(Widget& widget, std::string_view name, std::string_view value, Window& window) const;

private:
    void add(std::string name, Setter setter);

    std::map<std::string, Setter, std::less<>> setters_;
};

} // namespace ladoga::xml
