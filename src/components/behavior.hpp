/**
* @file behavior.hpp
 * @brief Defines the BehaviorComponent for attaching custom logic to entities.
 */
#pragma once

#include "../core/behaviors/icollision_responder.hpp"
#include <memory>

/**
 * @struct BehaviorComponent
 * @brief Attaches a polymorphic behavior to an entity, specifically for collision responses.
 *
 * This component acts as a bridge, allowing an entity to have custom, dynamically-assigned
 * logic. It uses a pointer to an interface (`ICollisionResponder`) to decouple the entity
 * from the concrete implementation of its behavior. This is a classic application of the
 * Strategy design pattern.
 *
 * For example, one entity might have a behavior to be destroyed on collision (like a pickup),
 * while another might have a behavior to open a door. A dedicated system can iterate
 * through entities with this component and invoke the appropriate behavior when a
 * collision event occurs. This approach replaces hard-coded logic in systems like
 * `GameplaySystem`, promoting better code organization and extensibility.
 */
//TODO: discuss if this is the right name for the component given it's responsability
struct BehaviorComponent {
    // This unique_ptr can hold any class that implements the ICollisionResponder interface.
    // This can hold any kind of collision response, C++ or scripted.
    std::unique_ptr<ICollisionResponder> responder;
};
