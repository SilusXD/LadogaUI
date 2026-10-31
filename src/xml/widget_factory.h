#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace ladoga {
class Widget;
} // namespace ladoga

namespace ladoga::xml {

/// Фабрика виджетов по имени XML-тега. std::map + std::function — этого достаточно.
class WidgetFactory {
public:
    WidgetFactory();

    /// Бросает std::invalid_argument для неизвестного тега.
    std::unique_ptr<Widget> create(std::string_view tag) const;

private:
    using Creator = std::function<std::unique_ptr<Widget>()>;

    void add(std::string tag, Creator creator);

    std::map<std::string, Creator, std::less<>> creators_;
};

} // namespace ladoga::xml
