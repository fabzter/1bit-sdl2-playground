#include "behavior_system.hpp"
#include "../components/behavior.hpp"
#include "../events/collision.hpp"

void BehaviorSystem::init(entt::registry& registry) {
    m_registry = &registry;
    auto& dispatcher = registry.ctx().get<entt::dispatcher>();
    dispatcher.sink<CollisionEvent>().connect<&BehaviorSystem::onCollision>(this);
}

void BehaviorSystem::onCollision(const CollisionEvent& event) {
    auto& registry = *m_registry;

    // Check if the first entity has a behavior to execute.
    if (auto* behavior = registry.try_get<BehaviorComponent>(event.a)) {
        if (behavior->responder) {
            behavior->responder->onCollision(event.a, event.b, registry);
        }
    }

    // Check if the second entity has a behavior to execute.
    if (auto* behavior = registry.try_get<BehaviorComponent>(event.b)) {
        if (behavior->responder) {
            behavior->responder->onCollision(event.b, event.a, registry);
        }
    }
}
