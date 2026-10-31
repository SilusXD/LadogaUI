#include "xml/widget_factory.h"

#include <stdexcept>
#include <utility>

#include <ladoga/button.h>
#include <ladoga/check_box.h>
#include <ladoga/label.h>
#include <ladoga/slider.h>
#include <ladoga/stack_panel.h>
#include <ladoga/text_box.h>
#include <ladoga/widget.h>

namespace ladoga::xml {

WidgetFactory::WidgetFactory() {
    add("StackPanel", [] { return std::make_unique<StackPanel>(); });
    add("Label", [] { return std::make_unique<Label>(); });
    add("Button", [] { return std::make_unique<Button>(); });
    add("TextBox", [] { return std::make_unique<TextBox>(); });
    add("CheckBox", [] { return std::make_unique<CheckBox>(); });
    add("Slider", [] { return std::make_unique<Slider>(); });
}

std::unique_ptr<Widget> WidgetFactory::create(std::string_view tag) const {
    const auto it = creators_.find(tag);
    if (it == creators_.end()) {
        throw std::invalid_argument("неизвестный тег <" + std::string(tag) + ">");
    }
    return it->second();
}

void WidgetFactory::add(std::string tag, Creator creator) {
    creators_[std::move(tag)] = std::move(creator);
}

} // namespace ladoga::xml
