#include "animation_state_system.hpp"
#include "../components/sprite.hpp"
#include "../components/blackboard.hpp"
#include "../core/blackboard_keys.hpp"
#include <iostream>
#include <entt/entt.hpp>

using namespace entt::literals;

void AnimationStateSystem::update(entt::registry& registry, InputManager& inputManager,
    ResourceManager& resourceManager, float deltaTime) {
    const auto view = registry.view<const BlackboardComponent, SpriteComponent>();

    for (const auto entity : view) {
        const auto& blackboard = view.get<const BlackboardComponent>(entity);
        auto& sprite = view.get<SpriteComponent>(entity);

        bool isMoving = false;
        if (auto it = blackboard.values.find(BlackboardKeys::State::IsMoving); it != blackboard.values.end()) {
            try {
                isMoving = std::any_cast<bool>(it->second);
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Warning: Blackboard key '" << BlackboardKeys::State::IsMoving << "' has wrong type." << std::endl;
            }
        }

        // ---- THIS IS THE OPTIMIZED LOGIC ----
        if (isMoving) {
            sprite.currentState = "walk"_hs;
        } else {
            sprite.currentState = "idle"_hs;
        }
    }
}
