#include <ladoga/version.h>

#ifndef LADOGA_VERSION_STRING
#define LADOGA_VERSION_STRING "0.1.0"
#endif

namespace ladoga {

std::string_view version() noexcept { return LADOGA_VERSION_STRING; }

} // namespace ladoga
