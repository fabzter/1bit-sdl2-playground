#pragma once

#include "../../core/fsm/istate.hpp"
#include "../../core/entt_helpers.hpp"
#include "transition_condition.hpp"
#include "transition.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <entt/entt.hpp>

struct StateMachineComponent {
    // A map of all states available to this FSM, keyed by their name.
    std::unordered_map<entt::hashed_string, std::unique_ptr<IState>> states;

    // The key of the currently active state in the map.
    entt::hashed_string currentState{""_hs};

    // The key of the previously active state.
    entt::hashed_string previousState{""_hs};

    // Time elapsed since entering the current state.
    float timeInState = 0.0f;

    // --- Special Member Functions to handle move-only semantics ---
    StateMachineComponent() = default;
    ~StateMachineComponent() = default;

    // By defaulting the move operations, we tell the compiler to generate
    // the correct member-wise move, which is what we want.
    StateMachineComponent(StateMachineComponent&& other) noexcept = default;
    StateMachineComponent& operator=(StateMachineComponent&& other) noexcept = default;

    // By explicitly deleting the copy operations, we make it clear
    // that this component cannot be copied, which resolves the compiler's ambiguity.
    StateMachineComponent(const StateMachineComponent&) = delete;
    StateMachineComponent& operator=(const StateMachineComponent&) = delete;
};
