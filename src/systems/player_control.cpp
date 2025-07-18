#include "player_control.hpp"
#include "../components/player_control.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include <iostream>

void PlayerControlSystem::update(entt::registry& registry, const InputManager& inputManager, float deltaTime) {
    auto view = registry.view<PlayerControlComponent, TransformComponent, SpriteComponent>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& sprite = view.get<SpriteComponent>(entity);

        float speed = 200.0f; // pixels per second
        bool isMoving = false;

        if (inputManager.isActionPressed("move_up")) {
            transform.position.y -= speed * deltaTime;
            isMoving = true;
        }
        if (inputManager.isActionPressed("move_down")) {
            transform.position.y += speed * deltaTime;
            isMoving = true;
        }
        if (inputManager.isActionPressed("move_left")) {
            transform.position.x -= speed * deltaTime;
            isMoving = true;
        }
        if (inputManager.isActionPressed("move_right")) {
            transform.position.x += speed * deltaTime;
            isMoving = true;
        }

        // Example of changing animation state based on input
        if (isMoving) {
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
