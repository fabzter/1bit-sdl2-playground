#pragma once

#include <string>
#include <cstdint>
#include <SDL2/SDL.h>
#include "../core/entt_helpers.hpp"

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
     * @brief The main sorting layer (e.g., Background, World, UI).
     * Higher numbers are drawn on top.
     */
    int16_t sortingLayer = 0;

    /**
     * @brief The sort order within a layer.
     * Higher numbers are drawn on top within the same sortingLayer.
     */
    int16_t orderInLayer = 0;

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
    entt::hashed_string currentState{"idle"_hs};
    
    /**
     * @brief The current frame index within the animation sequence.
     */
    int currentFrame = 0;
    
    /**
     * @brief Accumulator for timing frame transitions.
     */
    float animationTimer = 0.0f;

    SpriteComponent() = default;
    SpriteComponent(std::string id, int w, int h, int16_t layer = 0, int16_t order = 0)
        : assetId(std::move(id)), width(w), height(h), sortingLayer(layer), orderInLayer(order) {}

    /**
     * @brief Combines layer and order into a single key for efficient sorting.
     * @return A 32-bit integer where the high bits are the layer and the low bits are the order.
     */
    [[nodiscard]] int32_t getSortKey() const {
        return (static_cast<int32_t>(sortingLayer) << 16) | static_cast<uint16_t>(orderInLayer);
    }
};
