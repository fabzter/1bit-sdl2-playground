#include "top_down_movement_system.hpp"
#include "../components/intent.hpp"
#include "../components/transform.hpp"
#include "../components/movement.hpp"
#include "../components/blackboard.hpp"
#include "../core/blackboard_keys.hpp"
#include <iostream>

void TopDownMovementSystem::update(entt::registry& registry, float deltaTime) {
    // This system acts on any entity that has an intent, a transform, and movement stats.
    auto view = registry.view<const IntentComponent, TransformComponent,
                              const MovementComponent, BlackboardComponent>();

    for (auto entity : view) {
        const auto& intent = view.get<const IntentComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);
        const auto& movement = view.get<const MovementComponent>(entity);
        auto& blackboard = view.get<BlackboardComponent>(entity);

        // Apply the movement based on the intent and the entity's speed
        transform.position.x += movement.speed * intent.moveDirection.x * deltaTime;
        transform.position.y += movement.speed * intent.moveDirection.y * deltaTime;

        // Update blackboard based on intent
        blackboard.values[BlackboardKeys::State::IsMoving] = (intent.moveDirection.x != 0.0f || intent.moveDirection.y != 0.0f);

        // Handle one-shot actions from intent
        if (intent.actionJustPressed) {
            std::cout << "Action Button Processed by TopDownMovementSystem!" << std::endl;
        }
    }
}
