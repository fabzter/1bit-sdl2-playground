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
        if (!asset || !asset->texture) {
            std::cerr << "RenderSystem::draw - Asset not found for id: " << sprite.assetId << std::endl;
            continue;
        }

        SDL_Texture* texture = asset->texture.get();

        // Use component data to define where and how to draw the sprite
        SDL_FRect destRect = {
            transform.position.x + (screenW - sprite.width * transform.scale.x) / 2.0f,
            transform.position.y + (screenH - sprite.height * transform.scale.y) / 2.0f,
            sprite.width * transform.scale.x,
            sprite.height * transform.scale.y
        };

        SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
        SDL_SetTextureAlphaMod(texture, sprite.color.a);

        SDL_RenderCopyF(renderer, texture, NULL, &destRect);
    }
}
