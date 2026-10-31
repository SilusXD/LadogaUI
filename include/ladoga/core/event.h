#pragma once

#include <string>

#include <ladoga/core/geometry.h>

namespace ladoga {

enum class EventType {
    MouseDown,
    MouseUp,
    MouseMove,
    KeyDown,
    TextInput,
};

enum class MouseButton {
    Left,
    Middle,
    Right,
};

/// Нейтральное событие. SDL3 переводится в него в WindowBackend.
struct Event {
    EventType type = EventType::MouseDown;
    Point position{};
    MouseButton button = MouseButton::Left;
    int key = 0;
    std::string text;
};

} // namespace ladoga
