#include "physics_system.hpp"
#include "../components/transform.hpp"
#include "../components/rigidbody.hpp"

void PhysicsSystem::update(entt::registry& registry, InputManager&, ResourceManager&, float deltaTime) {
    auto view = registry.view<TransformComponent, const RigidBodyComponent>();

    // Instead of a structured binding, get each component separately.
    for (const auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        const auto& rigidbody = view.get<const RigidBodyComponent>(entity);

        transform.position.x += rigidbody.velocity.x * deltaTime;
        transform.position.y += rigidbody.velocity.y * deltaTime;
    }
}
