#include "top_down_kinematic_movement_system.hpp"
#include "../components/intent.hpp"
#include "../components/rigidbody.hpp"
#include "../components/transform.hpp"
#include "../components/movement.hpp"
#include "../components/blackboard.hpp"
#include "../core/blackboard_keys.hpp"
#include <iostream>

void TopDownKinematicMovementSystem::update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) {
    // This system acts on any entity that has an intent and movement stats.
    auto view = registry.view<const IntentComponent, RigidBodyComponent, const MovementComponent, BlackboardComponent>();

    for (auto entity : view) {
        const auto& intent = view.get<const IntentComponent>(entity);
        auto& rigidbody = view.get<RigidBodyComponent>(entity);
        const auto& movement = view.get<const MovementComponent>(entity);
        auto& blackboard = view.get<BlackboardComponent>(entity);

        //TODO: maybe movement systems should be separated although realted at the same time?
        if (rigidbody.bodyType == BodyType::KINEMATIC) {
            rigidbody.velocity.x = movement.speed * intent.moveDirection.x;
            rigidbody.velocity.y = movement.speed * intent.moveDirection.y;
        } else if (rigidbody.bodyType == BodyType::DYNAMIC) {
            //TODO: rhis magic number should no go here probably
            //TODO: is this really tha place ti update force? shouldn't we really be traing to cchange the intent? this is justa topic of discussion
            // Apply a force in the direction of intent.
            // The force is scaled to overcome friction/drag later if we add it.
            // For now, a large number ensures it feels responsive.
            rigidbody.force.x += movement.speed * 10.0f * intent.moveDirection.x;
            rigidbody.force.y += movement.speed * 10.0f * intent.moveDirection.y;
        }
    }
}
