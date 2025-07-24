#include "renderer.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/camera.hpp"
#include  "../core/context.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

namespace {
    // A simple struct to hold an entity and its calculated sort key.
    struct Renderable {
        entt::entity entity;
        int32_t sortKey;

        // Overload the less-than operator to allow direct sorting.
        bool operator<(const Renderable& other) const {
            return sortKey < other.sortKey;
        }
    };
}

void RenderSystem::draw(SDL_Renderer* renderer, entt::registry& registry,
    ResourceManager& resourceManager) {
    if (!registry.ctx().contains<ActiveCamera>()) return; // No active camera
    // --- get the Active Camera ---
    const auto cameraEntity = registry.ctx().get<ActiveCamera>().entity;
    if (!registry.valid(cameraEntity)) return; // Camera was destroyed

    // --- Camera calculation ---
    const auto& screen = registry.ctx().get<ScreenDimensions>();
    const auto& camTransform = registry.get<const TransformComponent>(cameraEntity);
    Vec2f cameraPos = camTransform.position;
    // The camera's position is its top-left corner in the world.
    // To center the view on the camera's anchor point, we offset by half the screen size.
    const float cameraOffsetX = cameraPos.x - screen.w / 2.0f;
    const float cameraOffsetY = cameraPos.y - screen.h / 2.0f;

    // --- COLLECT ---
    // Create the render queue for this frame.
    std::vector<Renderable> renderQueue;
    auto view = registry.view<const TransformComponent, const SpriteComponent>();
    renderQueue.reserve(view.size_hint()); // Pre-allocate memory
    for (const auto entity : view) {
        const auto& sprite = view.get<const SpriteComponent>(entity);
        renderQueue.emplace_back(Renderable{entity, sprite.getSortKey()});
    }

    // --- SORT ---
    // Sort the queue. Thanks to the operator< overload, it's a one-liner.
    // TODO: explore options to avoid sorting on every call
    std::sort(renderQueue.begin(), renderQueue.end());

    // --- DRAW ---
    // Iterate over each entity in the view
    for (const auto renderable : renderQueue) {
        // Get the components from the renderable's entity
        const auto& transform = registry.get<const TransformComponent>(renderable.entity);
        const auto& sprite = registry.get<const SpriteComponent>(renderable.entity);

        const SpriteAsset* asset = resourceManager.getSpriteAsset(sprite.assetId);
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
            transform.position.x - cameraOffsetX, // Apply camera offset
            transform.position.y - cameraOffsetY, // Apply camera offset
            static_cast<float>(sprite.width) * transform.scale.x,
            static_cast<float>(sprite.height) * transform.scale.y
        };

        SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
        SDL_SetTextureAlphaMod(texture, sprite.color.a);
        SDL_RenderCopyF(renderer, texture, &srcRect, &destRect);
    }
}
