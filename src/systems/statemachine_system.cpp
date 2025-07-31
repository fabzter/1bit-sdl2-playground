#include "statemachine_system.hpp"
#include "../components/statemachine/statemachine.hpp"
#include "../components/blackboard.hpp"
#include "../core/blackboard_keys.hpp"
#include <entt/entt.hpp>

using namespace entt::literals;

void StateMachineSystem::update(entt::registry& registry, InputManager&, ResourceManager&, float deltaTime) {
    auto view = registry.view<StateMachineComponent, const BlackboardComponent>();

    for (const auto entity : view) {
        auto& fsm = view.get<StateMachineComponent>(entity);
        const auto& blackboard = view.get<const BlackboardComponent>(entity);

        // --- 1. Check for a valid transition ---
        entt::hashed_string nextStateKey{""_hs};

        // Find the list of possible transitions from the current state in the TRANSITIONS map.
        if (auto it = fsm.transitions.find(fsm.currentState); it != fsm.transitions.end()) {
            const auto& transitionsForState = it->second;
            for (const auto& transition : transitionsForState) {
                bool allConditionsMet = true;
                // Check all conditions for this transition.
                for (const auto& condition : transition.conditions) {
                    bool conditionMet = false;
                    if (auto bb_it = blackboard.values.find(condition.blackboardKey); bb_it != blackboard.values.end()) {
                        // Check if the blackboard value matches the condition's expected value.
                        if (const bool* value = std::any_cast<bool>(&bb_it->second)) {
                            if (*value == condition.expectedValue) {
                                conditionMet = true;
                            }
                        }
                    }
                    // If any condition is not met, this transition is invalid.
                    if (!conditionMet) {
                        allConditionsMet = false;
                        break;
                    }
                }

                if (allConditionsMet) {
                    nextStateKey = transition.toState;
                    break; // Found a valid transition, stop checking others.
                }
            }
        }

        // --- 2. Perform State Change if Needed ---
        if (nextStateKey != ""_hs && nextStateKey != fsm.currentState) {
            if (auto oldStateIt = fsm.states.find(fsm.currentState); oldStateIt != fsm.states.end()) {
                oldStateIt->second->onExit(entity, registry);
            }

            fsm.previousState = fsm.currentState;
            fsm.currentState = nextStateKey;
            fsm.timeInState = 0.0f;

            if (auto newStateIt = fsm.states.find(fsm.currentState); newStateIt != fsm.states.end()) {
                newStateIt->second->onEnter(entity, registry);
            }
        }

        // --- 3. Update the Current State ---
        fsm.timeInState += deltaTime;
        if (auto currentStateIt = fsm.states.find(fsm.currentState); currentStateIt != fsm.states.end()) {
            currentStateIt->second->onUpdate(entity, registry, deltaTime);
        }
    }
}