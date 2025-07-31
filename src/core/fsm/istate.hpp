#pragma once

#include <entt/entt.hpp>

/**
 * @class IState
 * @brief An interface that defines the contract for a single state in a state machine.
 */
class IState {
public:
    virtual ~IState() = default;

    /**
     * @brief Called once when the state machine enters this state.
     * @param entity The entity that owns this state machine.
     * @param registry The scene's entity registry.
     */
    virtual void onEnter(entt::entity entity, entt::registry& registry) = 0;

    /**
     * @brief Called every frame while this state is active.
     * @param entity The entity that owns this state machine.
     * @param registry The scene's entity registry.
     * @param deltaTime The time since the last frame.
     */
    virtual void onUpdate(entt::entity entity, entt::registry& registry, float deltaTime) = 0;

    /**
     * @brief Called once when the state machine exits this state.
     * @param entity The entity that owns this state machine.
     * @param registry The scene's entity registry.
     */
    virtual void onExit(entt::entity entity, entt::registry& registry) = 0;
};
