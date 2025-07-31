#pragma once

#include "istate.hpp"
#include "../../components/sprite.hpp"
#include "../../core/entt_helpers.hpp" // For hashed_string

class SimpleAnimationState : public IState {
public:
    // We construct the state with the ID of the animation it should play.
    explicit SimpleAnimationState(entt::hashed_string animName) 
        : m_animationName(animName) {}

    void onEnter(entt::entity entity, entt::registry& registry) override {
        if (auto* sprite = registry.try_get<SpriteComponent>(entity)) {
            sprite->currentState = m_animationName;
            sprite->currentFrame = 0; // Always restart the animation
            sprite->animationTimer = 0.0f;
        }
    }

    // This state has no per-frame logic.
    void onUpdate(entt::entity, entt::registry&, float) override {}

    // This state has no cleanup logic.
    void onExit(entt::entity, entt::registry&) override {}

private:
    entt::hashed_string m_animationName;
};
