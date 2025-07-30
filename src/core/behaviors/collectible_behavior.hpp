/**
* @file collectible_behavior.hpp
 * @brief Defines the behavior for a collectible item that is destroyed on collision with the player.
 */
#pragma once

#include "icollision_responder.hpp"
#include "../../components/player_control.hpp"
#include <iostream>

/**
 * @class CollectibleBehavior
 * @brief A concrete collision responder for collectible items.
 *
 * This class implements the `ICollisionResponder` interface to define the
 * behavior of a collectible entity (like a coin or a power-up). When an entity
 * with this behavior collides with another entity, this behavior checks if the
 * other entity is the player (by looking for a `PlayerControlComponent`).
 *
 * If the collision is with the player, the collectible entity is destroyed.
 * This provides a simple and reusable way to create "pickup" items in the game.
 */
class CollectibleBehavior : public ICollisionResponder {
public:
    /**
     * @brief Handles the collision event for the collectible.
     *
     * This method is called by the `BehaviorSystem` when a collision involving
     * the `self` entity occurs. It checks if the `other` entity has a
     * `PlayerControlComponent`. If it does, it destroys the `self` entity.
     *
     * @param self The entity that owns this behavior (the collectible).
     * @param other The entity that `self` collided with.
     * @param registry The game's entity registry, used to check for components and destroy entities.
     */
    void onCollision(entt::entity self, entt::entity other, entt::registry& registry) override {
        // Only trigger if the "other" entity is the player
        if (registry.all_of<PlayerControlComponent>(other)) {
            std::cout << "Collected entity " << static_cast<uint32_t>(self) << "!" << std::endl;
            // Add to score, play sound, etc.
            registry.destroy(self);
        }
    }
};
