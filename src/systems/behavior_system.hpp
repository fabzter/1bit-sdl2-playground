/**
* @file behavior_system.hpp
 * @brief Defines the BehaviorSystem, which processes collision events and delegates responses.
 */
#pragma once

#include "../core/systems/isystem.hpp"
#include <entt/entt.hpp>

struct CollisionEvent;

/**
 * @class BehaviorSystem
 * @brief A system that handles collision events by delegating to BehaviorComponents.
 *
 * This system subscribes to `CollisionEvent`s dispatched within the application.
 * When a collision occurs, it checks if either of the involved entities has a
 * `BehaviorComponent`. If a component is found, the system invokes the `onCollision`
 * method of the `ICollisionResponder` held by that component.
 *
 * This design effectively decouples collision response logic from monolithic systems
 * (like a `GameplaySystem`). Instead of hard-coding "if player hits coin, destroy coin",
 * you can attach a `DestroyOnCollisionBehavior` to the coin entity. This makes the
 * collision handling logic data-driven, modular, and easily extensible without modifying
 * the core systems.
 *
 * @note This system's primary logic is event-driven via the `onCollision` handler.
 * The `update` method is implemented to satisfy the `IUpdateSystem` interface but
 * is currently empty.
 */
//TODO: discuss if this is the right name for the behavior given it's responsability
class BehaviorSystem : public IUpdateSystem {
public:
    /**
     * @brief Initializes the system and subscribes to collision events.
     * @param registry The central entity-component-system registry.
     */
    void init(entt::registry& registry) override;
    /**
     * @brief The per-frame update function.
     * @note This is currently a no-op ( for now)  as the system is event-driven.
     */
    void update(entt::registry&, InputManager&, ResourceManager&, float) override {}

private:
    /**
    * @brief Event handler called when a CollisionEvent is dispatched.
    *
    * Checks both entities in the event for a `BehaviorComponent` and executes
    * their `onCollision` responder if it exists.
    * @param event The data for the collision that occurred.
    */
    void onCollision(const CollisionEvent& event);

    /**
     * @brief A cached pointer to the registry.
     *
     * Stored during `init` to avoid passing it repeatedly and to allow event
     * handlers to access it.
     */
    entt::registry* m_registry = nullptr;
};
