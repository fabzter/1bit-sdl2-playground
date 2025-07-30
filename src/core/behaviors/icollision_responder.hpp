/**
* @file icollision_responder.hpp
 * @brief Defines the ICollisionResponder interface for handling collision responses.
 */
#pragma once

#include <entt/entt.hpp>

/**
 * @class ICollisionResponder
 * @brief An interface for components that need to define a custom response to a collision event.
 *
 * This class provides a contract for any component that wishes to react to collisions.
 * By implementing this interface, a component can encapsulate its own collision logic,
 * making the system more modular and easier to maintain. A separate system can then
 * iterate over components that implement this interface and invoke their `onCollision` method.
 *
 * This approach decouples collision detection from collision response, promoting better
 * separation of concerns. For example, one component might implement this to destroy
 * itself on collision (like a pickup), while another might implement it to deal damage.
 */
class ICollisionResponder {
public:
    virtual ~ICollisionResponder() = default;

    /**
     * @brief The function that gets called when a collision occurs.
     * @param self The entity that owns this behavior component.
     * @param other The other entity involved in the collision.
     * @param registry The scene's entity registry.
     */
    virtual void onCollision(entt::entity self, entt::entity other, entt::registry& registry) = 0;
};
