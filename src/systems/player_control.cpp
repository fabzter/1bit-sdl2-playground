#include "player_control.hpp"
#include "../components/player_control.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/movement.hpp"
#include <iostream>

void PlayerControlSystem::update(entt::registry& registry, const InputManager& inputManager, float deltaTime) {
    auto view = registry.view<PlayerControlComponent, TransformComponent, SpriteComponent, MovementComponent>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& sprite = view.get<SpriteComponent>(entity);
        auto& movement = view.get<MovementComponent>(entity);

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

        // --- Apply Movement ---
        transform.position.x += movement.speed * moveX * deltaTime;
        transform.position.y += movement.speed * moveY * deltaTime;

        // --- Update Animation State ---
        //TODO: question: is the work of this system to update the state? just something to discuss about
        
        // Use squared length to avoid sqrt, which is more efficient.
        const float magnitudeSq = moveX * moveX + moveY * moveY;
        if (magnitudeSq > 0.01f) { // Check if we are moving
            sprite.currentState = "walk";
        } else {
            sprite.currentState = "idle";
        }

        // Example of using a "just pressed" event for a one-shot action
        if (inputManager.isActionJustPressed("action_button")) {
            std::cout << "Action Button Pressed!" << std::endl;
            // E.g., trigger a jump, shoot, or interact
        }
    }
}
