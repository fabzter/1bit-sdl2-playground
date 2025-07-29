#include "gameplay_system.hpp"
#include "../components/player_control.hpp"
#include "../components/collider.hpp"
#include <entt/entt.hpp>
#include <iostream>
using namespace entt;

void GameplaySystem::init(entt::registry& registry) {
    m_registry = &registry; // <-- Store the registry pointer
    auto& dispatcher = registry.ctx().get<entt::dispatcher>();
    // Connect our onCollision method to the dispatcher.
    // Now, whenever a CollisionEvent is dispatched, this method will be called.
    dispatcher.sink<CollisionEvent>().connect<&GameplaySystem::onCollision>(this);
}

void GameplaySystem::onCollision(const CollisionEvent& event) {
    auto& registry = *m_registry; // <-- Use the stored pointer

    entt::entity otherEntity = entt::null;
    if (registry.all_of<PlayerControlComponent>(event.a)) {
        otherEntity = event.b;
    } else if (registry.all_of<PlayerControlComponent>(event.b)) {
        otherEntity = event.a;
    }

    if (otherEntity != entt::null) {
        const auto& collider = registry.get<ColliderComponent>(otherEntity);
        if (collider.is_trigger) {
            std::cout << "SUCCESS: Player collided with a trigger entity ("
                      << static_cast<uint32_t>(otherEntity) << ")!" << std::endl;

            registry.destroy(otherEntity);
        }
    }
}
