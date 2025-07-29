#include "top_down_movement_system.hpp"
#include "../components/intent.hpp"
#include "../components/rigidbody.hpp"
#include "../components/transform.hpp"
#include "../components/movement.hpp"
#include "../components/blackboard.hpp"
#include "../core/blackboard_keys.hpp"
#include <iostream>

void TopDownMovementSystem::update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) {
    // This system acts on any entity that has an intent and movement stats.
    auto view = registry.view<const IntentComponent, RigidBodyComponent,
        const MovementComponent, BlackboardComponent>();

    for (auto entity : view) {
        const auto& intent = view.get<const IntentComponent>(entity);
        auto& rigidbody = view.get<RigidBodyComponent>(entity);
        const auto& movement = view.get<const MovementComponent>(entity);
        auto& blackboard = view.get<BlackboardComponent>(entity);

        // Set the velocity based on the intent and the entity's speed.
        rigidbody.velocity.x = movement.speed * intent.moveDirection.x;
        rigidbody.velocity.y = movement.speed * intent.moveDirection.y;

        // Update blackboard based on intent
        blackboard.values[BlackboardKeys::State::IsMoving] = (intent.moveDirection.x != 0.0f || intent.moveDirection.y != 0.0f);

        // Update blackboard based on intent.
        //TODO: This is a good candidate for a separate MovementStateSystem later.
        blackboard.values[BlackboardKeys::State::IsMoving] = (intent.moveDirection.x != 0.0f || intent.moveDirection.y != 0.0f);
    }
}
