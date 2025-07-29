#include "physics_system.hpp"
#include "../components/transform.hpp"
#include "../components/rigidbody.hpp"

void PhysicsSystem::update(entt::registry& registry, InputManager&, ResourceManager&, float deltaTime) {
    auto view = registry.view<TransformComponent, const RigidBodyComponent>();

    for (const auto entity : view) {
        auto const& [transform, rigidbody] = view.get<TransformComponent, const RigidBodyComponent>(entity);
        transform.position.x += rigidbody.velocity.x * deltaTime;
        transform.position.y += rigidbody.velocity.y * deltaTime;
    }
}
