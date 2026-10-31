#pragma once

#include <ladoga/widget.h>

namespace ladoga {

enum class Orientation { Vertical, Horizontal };

/// Линейная раскладка детей: как QVBoxLayout/QHBoxLayout или WPF StackPanel.
class StackPanel : public Widget {
public:
    Orientation orientation() const noexcept;
    void set_orientation(Orientation value);

    Size preferred_size() const override;

protected:
    void layout_children() override;

private:
    Orientation orientation_ = Orientation::Vertical;
};

} // namespace ladoga
