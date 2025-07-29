#include "gameplay_system.hpp"
#include <iostream>

void GameplaySystem::init(entt::registry& registry) {
    auto& dispatcher = registry.ctx().get<entt::dispatcher>();
    // Connect our onCollision method to the dispatcher.
    // Now, whenever a CollisionEvent is dispatched, this method will be called.
    dispatcher.sink<CollisionEvent>().connect<&GameplaySystem::onCollision>(this);
}

void GameplaySystem::onCollision(const CollisionEvent& event) {
    // For now, just print a message to prove it's working.
    // Later, we can check what entities collided and trigger game logic.
    std::cout << "GAMEPLAY EVENT: Collision between entity " << static_cast<uint32_t>(event.a)
              << " and entity " << static_cast<uint32_t>(event.b) << std::endl;
}
