#include "statemachine_system.hpp"
#include "../components/statemachine.hpp"
#include "../components/blackboard.hpp"
#include "../core/blackboard_keys.hpp"

void StateMachineSystem::update(entt::registry& registry, InputManager&, ResourceManager&, float deltaTime) {
    auto view = registry.view<StateMachineComponent, const BlackboardComponent>();

    for (const auto entity : view) {
        auto& stateMachine = view.get<StateMachineComponent>(entity);
        const auto& blackboard = view.get<const BlackboardComponent>(entity);

        // --- 1. Check for Transitions (Temporary Logic) ---
        // TODO: This logic will be replaced by a data-driven transition system.
        // For now, we are hardcoding the idle <-> walk transition.
        bool isMoving = false;
        if (auto it = blackboard.values.find(BlackboardKeys::State::IsMoving); it != blackboard.values.end()) {
            isMoving = std::any_cast<bool>(it->second);
        }

        //TODO: should the sistem itself now about the states themselves? in a stat machine a state is justa node and then we have an event that transitions them.
        entt::hashed_string newStateKey = isMoving ? "walk"_hs : "idle"_hs;

        if (newStateKey != stateMachine.currentState) {
            // A state transition is needed
            IState* oldState = stateMachine.states[stateMachine.currentState].get();
            if (oldState) {
                oldState->onExit(entity, registry);
            }

            stateMachine.previousState = stateMachine.currentState;
            stateMachine.currentState = newStateKey;
            stateMachine.timeInState = 0.0f;

            IState* newState = stateMachine.states[stateMachine.currentState].get();
            if (newState) {
                newState->onEnter(entity, registry);
            }
        }

        // --- 2. Update the Current State ---
        stateMachine.timeInState += deltaTime;
        if (auto* currentState = stateMachine.states[stateMachine.currentState].get()) {
            currentState->onUpdate(entity, registry, deltaTime);
        }
    }
}
