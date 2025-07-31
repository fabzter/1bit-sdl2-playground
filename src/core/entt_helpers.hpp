#pragma once

#include <entt/entt.hpp>

// This allows us to use the "string"_hs syntax in any file that includes this header.
using namespace entt::literals;

// This is the template specialization that teaches std::hash how to work with entt::hashed_string.
// By placing it here, we ensure it's available wherever we use hashed_strings as map keys.
template <>
struct std::hash<entt::hashed_string> {
    std::size_t operator()(const entt::hashed_string& hs) const {
        return hs.value();
    }
};
