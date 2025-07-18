#include "renderer.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include <iostream>

RenderSystem::~RenderSystem() {
}

void RenderSystem::draw(SDL_Renderer* renderer, entt::registry& registry,
    ResourceManager& resourceManager) {
    // Get the screen dimensions to center entities if needed
    int screenW = 0, screenH = 0;
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);

    // Create a view of all entities that have both a Transform and a Sprite component
    auto view = registry.view<const TransformComponent, const SpriteComponent>();

    // Iterate over each entity in the view
    for (const auto entity : view) {
        // Get the components from the entity
        const auto& transform = view.get<const TransformComponent>(entity);
        const auto& sprite = view.get<const SpriteComponent>(entity);

        const SpriteAsset* asset = resourceManager.getSpriteAsset(renderer, sprite.assetId);
        if (!asset) {
            std::cerr << "RenderSystem::draw - Asset not found for id: " << sprite.assetId << std::endl;
            continue;
        }

        auto* texture = asset->textureAtlas.get();
        if (!texture) {
            std::cerr << "RenderSystem::draw - Asset not found for id: " << sprite.assetId << std::endl;
            continue;
        }

        // --- Calculate Source Rectangle ---
        int frameIndexInAtlas = 0; // 0 if anything fails
        // Find the animation sequence for the sprite's current state
        auto anim_it = asset->animations.find(sprite.currentState);
        if (anim_it != asset->animations.end()) {
            const AnimationSequence& sequence = anim_it->second;
            // Ensure the currentFrame index is valid for the sequence
            if (!sequence.empty() && sprite.currentFrame < sequence.size()) {
                // Get the correct frame index from the animation data
                frameIndexInAtlas = sequence[sprite.currentFrame].frameIndexInAtlas;
            }
        }

        // This selects which frame to draw from the atlas.
        // We assume all frames are laid out horizontally.
        const SDL_Rect srcRect = {
            frameIndexInAtlas * sprite.width,   // x position in the atlas
            0,                                  // y position (0 if all in one row)
            sprite.width,                       // width of one frame
            sprite.height                       // height of one frame
        };

        // Use component data to define where and how to draw the sprite
        const SDL_FRect destRect = {
            transform.position.x + (screenW - sprite.width * transform.scale.x) / 2.0f,
            transform.position.y + (screenH - sprite.height * transform.scale.y) / 2.0f,
            static_cast<float>(sprite.width) * transform.scale.x,
            static_cast<float>(sprite.height) * transform.scale.y
        };

        SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
        SDL_SetTextureAlphaMod(texture, sprite.color.a);

        SDL_RenderCopyF(renderer, texture, &srcRect, &destRect);
    }
}
