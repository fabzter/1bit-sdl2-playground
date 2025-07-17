#pragma once

#include <string>
#include <SDL2/SDL.h>

/**
 * @struct SpriteComponent
 * @brief Holds data required to render an entity as a sprite.
 * This includes a reference to the asset, dimensions, layer depth, and animation state.
 */
struct SpriteComponent {
    /**
     * @brief The unique identifier for the asset (e.g., "player").
     * The RenderSystem will use this ID to fetch the actual texture from a resource manager.
     */
    std::string assetId;
    
    int width = 0;
    int height = 0;

    /**
     * @brief The rendering order. Lower numbers are drawn first (further back).
     */
    int zIndex = 0;

    /**
     * @brief Color tint to apply to the sprite. Defaults to opaque white (no tint).
     */
    SDL_Color color{255, 255, 255, 255};

    // --- Animation Data ---
    
    bool isAnimated = false;

    /**
     * @brief The name of the current animation state (e.g., "idle", "walk").
     * The AnimationSystem uses this to find the correct animation sequence.
     */
    std::string currentState = "idle";
    
    /**
     * @brief The current frame index within the animation sequence.
     */
    int currentFrame = 0;
    
    /**
     * @brief Accumulator for timing frame transitions.
     */
    float animationTimer = 0.0f;

    SpriteComponent() = default;
    SpriteComponent(std::string id, int w, int h, int z = 0)
        : assetId(std::move(id)), width(w), height(h), zIndex(z) {}
};