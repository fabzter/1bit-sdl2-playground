#include "animation.hpp"
#include "../util/sprite_asset_loader.hpp"
#include "../components/sprite.hpp"

void AnimationSystem::update(entt::registry& registry, InputManager& inputManager,
    ResourceManager& resourceManager, float deltaTime) {
    auto view = registry.view<SpriteComponent>();

    for (const auto entity : view) {
        auto& sprite = view.get<SpriteComponent>(entity);

        // Skip entities that are not animated
        if (!sprite.isAnimated) {
            continue;
        }

        // The renderer is not needed for animation logic, so we can pass nullptr.
        const SpriteAsset* asset = resourceManager.getSpriteAsset(sprite.assetId);
        if (!asset) {
            continue;
        }

        // Find the animation sequence for the sprite's current state
        auto it = asset->animations.find(sprite.currentState);
        if (it == asset->animations.end() || it->second.empty()) {
            continue; // No animation found for this state
        }

        const AnimationSequence& sequence = it->second;

        // Add elapsed time to the animation timer
        sprite.animationTimer += deltaTime;

        // Get the duration of the current frame
        const int currentFrameDurationMs = sequence[sprite.currentFrame].durationMs;

        // Check if it's time to advance to the next frame
        if (sprite.animationTimer * 1000.0f >= currentFrameDurationMs) {
            sprite.animationTimer = 0.0f; // Reset timer
            // Advance to the next frame, looping back to the start if at the end
            sprite.currentFrame = (sprite.currentFrame + 1) % sequence.size();
        }
    }
}