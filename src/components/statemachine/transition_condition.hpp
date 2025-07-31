#pragma once
#include <string>

// Represents a condition that must be met for a transition to occur.
// For now, it only supports checking boolean values on the blackboard.
struct TransitionCondition {
    std::string tag;
    std::string name;
    std::string blackboardKey;
    bool expectedValue;
};
