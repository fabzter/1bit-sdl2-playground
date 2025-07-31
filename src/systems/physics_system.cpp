#include "physics_system.hpp"
#include "../components/transform.hpp"
#include "../components/rigidbody.hpp"

//TODO: how can we refactor or make cleaner the physic system now that is has more cases and is super more complex
void PhysicsSystem::update(entt::registry& registry, InputManager&, ResourceManager&, float deltaTime) {
    auto view = registry.view<TransformComponent, RigidBodyComponent>();

    for (const auto entity : view) {
        // Get each component individually to get mutable references.
        auto& transform = view.get<TransformComponent>(entity);
        auto& rigidbody = view.get<RigidBodyComponent>(entity);

        if (rigidbody.bodyType == BodyType::STATIC) {
            continue; // Static bodies don't move
        }
        if (rigidbody.bodyType == BodyType::DYNAMIC) {
            // DYNAMIC bodies use the force integrator and damping
            // 1. Apply forces to velocity (Integrate)
            // Using mass: a = F/m.  v += a * dt
            if (rigidbody.mass > 0.0f) {
                rigidbody.velocity.x += (rigidbody.force.x / rigidbody.mass) * deltaTime;
                rigidbody.velocity.y += (rigidbody.force.y / rigidbody.mass) * deltaTime;
            }
            //2. Apply damping
            rigidbody.velocity.x *= rigidbody.damping;
            rigidbody.velocity.y *= rigidbody.damping;
        }

        // For BOTH KINEMATIC and DYNAMIC, we apply the final velocity to the position.
        transform.position.x += rigidbody.velocity.x * deltaTime;
        transform.position.y += rigidbody.velocity.y * deltaTime;

        // 3. Clear forces for the next frame
        rigidbody.force = {0.0f, 0.0f};
    }
}
