#pragma once

#include <entt/entt.hpp>
#include <vector>
#include <string>
#include "transition_condition.hpp"

// Represents a potential transition from one state to another.
struct Transition {
    std::string tag;
    std::string name;
    entt::hashed_string toState;
    std::vector<TransitionCondition> conditions;
};
