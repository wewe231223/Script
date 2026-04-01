#pragma once
#include <concepts>
#include <vector>
#include <type_traits>

namespace Game {
    struct ComponentInspectionField;
}

template <typename T>
concept TrivialComponent =
std::is_trivially_copyable_v<T> &&
std::is_trivially_destructible_v<T> &&
std::is_standard_layout_v<T>;

#define Component(TypeName) struct TypeName { 

#define EndComponent(TypeName) \
    }; \
    static_assert(TrivialComponent<TypeName>, \
    "\n\n[FATAL ERROR] Invalid Component Layout: " #TypeName \
    "\n- Reason: All components must be Trivially Copyable and have Standard Layout.\n");
