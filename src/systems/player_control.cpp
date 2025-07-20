#include "player_control.hpp"
#include "../components/player_control.hpp"
#include "../components/action_intent.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/movement.hpp"
#include <iostream>

void PlayerControlSystem::update(entt::registry& registry, const InputManager& inputManager,
    float deltaTime) {
    auto view = registry.view<PlayerControlComponent, ActionIntentComponent>();

    for (auto entity : view) {
        auto& intent = view.get<ActionIntentComponent>(entity);

        // Reset intent for this frame
        intent.moveDirection = {0.0f, 0.0f};
        intent.actionJustPressed = false;

        // --- Get Axis Input ---
        // For controllers, this comes directly from the analog stick.
        float moveX = inputManager.getAxisValue("move_horizontal");
        float moveY = inputManager.getAxisValue("move_vertical");

        // --- Emulate Axis Input for Keyboard ---
        // If there's no controller input, check keyboard state actions.
        if (std::abs(moveX) < 0.1f && std::abs(moveY) < 0.1f) {
            if (inputManager.isActionPressed("move_right")) moveX = 1.0f;
            if (inputManager.isActionPressed("move_left")) moveX = -1.0f;
            if (inputManager.isActionPressed("move_down")) moveY = 1.0f;
            if (inputManager.isActionPressed("move_up")) moveY = -1.0f;
        }

        // --- Normalize and Store Intent ---
        float magnitude = std::sqrt(moveX * moveX + moveY * moveY);
        if (magnitude > 1.0f) {
            moveX /= magnitude;
            moveY /= magnitude;
        }
        intent.moveDirection = {moveX, moveY};

        // --- Store State Actions ---
        if (inputManager.isActionJustPressed("action_button")) {
            intent.actionJustPressed = true;
        }
    }
}
