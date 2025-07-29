#include "renderer.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include  "../core/context.hpp"
#include "../util/resource_manager.hpp"
#include "../util/sprite_asset.hpp"
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

void RenderSystem::onSpriteUpdate(entt::registry&, entt::entity) {
    m_isDirty = true;
}

void RenderSystem::init(entt::registry& registry) {
    // When a SpriteComponent is added or updated, mark our render queue as dirty.
    // The lambda captures `this` to modify the m_isDirty flag.
    auto onUpdate = [this](entt::registry&, entt::entity) {
        this->m_isDirty = true;
    };

    registry.on_construct<SpriteComponent>().connect<&RenderSystem::onSpriteUpdate>(this);
    registry.on_update<SpriteComponent>().connect<&RenderSystem::onSpriteUpdate>(this);
    registry.on_destroy<SpriteComponent>().connect<&RenderSystem::onSpriteUpdate>(this);
}

void RenderSystem::draw(SDL_Renderer* renderer, entt::registry& registry,
        ResourceManager& resourceManager) {
    if (!registry.ctx().contains<ActiveCamera>()) return; // No active camera
    // --- get the Active Camera ---
    const auto cameraEntity = registry.ctx().get<ActiveCamera>().entity;
    if (!registry.valid(cameraEntity)) return; // Camera was destroyed

    // ---- SORTING OPTIMIZATION ----
    if (m_isDirty) {
        // 1. Collect all entities with a SpriteComponent.
        auto view = registry.view<const SpriteComponent>();
        std::vector<Renderable> tempQueue;
        tempQueue.reserve(view.size());
        for (const auto entity : view) {
            const auto& sprite = view.get<const SpriteComponent>(entity);
            tempQueue.emplace_back(Renderable{entity, sprite.getSortKey()});
        }

        // 2. Sort the temporary list.
        std::sort(tempQueue.begin(), tempQueue.end());

        // 3. Update the persistent render queue.
        m_renderQueue.clear();
        m_renderQueue.reserve(tempQueue.size());
        for (const auto& renderable : tempQueue) {
            m_renderQueue.push_back(renderable.entity);
        }

        // 4. Mark as clean until the next change.
        m_isDirty = false;
    }
    // ----------------------------


    // --- Camera calculation ---
    const auto& screen = registry.ctx().get<ScreenDimensions>();
    const auto& camTransform = registry.get<const TransformComponent>(cameraEntity);
    Vec2f cameraPos = camTransform.position;
    // The camera's position is its top-left corner in the world.
    // To center the view on the camera's anchor point, we offset by half the screen size.
    const float cameraOffsetX = cameraPos.x - screen.w / 2.0f;
    const float cameraOffsetY = cameraPos.y - screen.h / 2.0f;

    // --- DRAW ---
    // Iterate over each entity in the view
    for (const auto entity : m_renderQueue) {
        // Get the components from the renderable's entity
        const auto& transform = registry.get<const TransformComponent>(entity);
        const auto& sprite = registry.get<const SpriteComponent>(entity);

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

        const float scaledWidth = static_cast<float>(sprite.width) * transform.scale.x;
        const float scaledHeight = static_cast<float>(sprite.height) * transform.scale.y;

        // Use component data to define where and how to draw the sprite
        // To draw a sprite centered on the transform's position, we must
        // offset the top-left drawing corner by half of the sprite's scaled size.
        const SDL_FRect destRect = {
            transform.position.x - (scaledWidth / 2.0f) - cameraOffsetX,
            transform.position.y - (scaledHeight / 2.0f) - cameraOffsetY,
            scaledWidth,
            scaledHeight
        };

        SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
        SDL_SetTextureAlphaMod(texture, sprite.color.a);
        SDL_RenderCopyF(renderer, texture, &srcRect, &destRect);
    }
}
