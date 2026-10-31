#pragma once

#include <string_view>

namespace ladoga {

inline constexpr int version_major = 0;
inline constexpr int version_minor = 1;
inline constexpr int version_patch = 0;

/// Версия фреймворка в виде "0.1.0".
std::string_view version() noexcept;

} // namespace ladoga
