#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

// This is a template specialization that teaches std::hash how to work with entt::hashed_string.
// It's required for using hashed_string as a key in std::unordered_map.
template <>
struct std::hash<entt::hashed_string> {
    std::size_t operator()(const entt::hashed_string& hs) const {
        return hs.value();
    }
};

// Forward-declare the custom deleter
struct SDL_Texture_Deleter {
    void operator()(SDL_Texture* texture) const;
};

/**
 * @struct AnimationFrame
 * @brief Defines a single frame of an animation, including its duration.
 */
struct AnimationFrame {
    int frameIndexInAtlas = 0; // The index of this frame within the texture atlas (e.g., 0, 1, 2...).
    int durationMs = 100;      // How long this frame should be displayed, in milliseconds.
};

// An AnimationSequence is a vector of frames that defines a complete animation for one state.
using AnimationSequence = std::vector<AnimationFrame>;

/**
 * @struct SpriteAsset
 * @brief A container for all data related to a single sprite asset.
 * This includes the SDL_Texture and metadata like its original size,
 * which are loaded from a .sprite file.
 */
struct SpriteAsset {
    std::string assetId;
    int width = 0;
    int height = 0;

    // A single texture containing all frames laid out horizontally.
    std::unique_ptr<SDL_Texture, SDL_Texture_Deleter> textureAtlas;

    // Maps a state name (e.g., "idle", "walk") to its animation sequence.
    std::unordered_map<entt::hashed_string, AnimationSequence> animations;
};