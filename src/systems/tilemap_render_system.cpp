#include "tilemap_render_system.hpp"
#include "../components/tilemap.hpp"
#include "../components/camera.hpp"
#include "../components/transform.hpp"
#include "../core/context.hpp"
#include "../util/resource_manager.hpp"
#include <algorithm>
#include <iostream>

void TilemapRenderSystem::draw(SDL_Renderer* renderer, entt::registry& registry,
        ResourceManager& resourceManager) {
    // There should only be one tilemap entity, find it.
    auto mapView = registry.view<TilemapComponent>();
    if (mapView.empty()) {
        return; // No tilemap in the scene
    }
    entt::entity tilemapEntity = mapView.front();
    const auto& tilemap = mapView.get<TilemapComponent>(tilemapEntity);

    // Get the tileset asset
    const TilesetAsset* tileset = resourceManager.getTilesetAsset(tilemap.tilesetAssetId);
    if (!tileset || !tileset->textureAtlas) {
        return; // Tileset not loaded or invalid
    }
    
    // Get camera information
    if (!registry.ctx().contains<ActiveCamera>()) return;
    const auto cameraEntity = registry.ctx().get<ActiveCamera>().entity;
    if (!registry.valid(cameraEntity)) return;

    const auto& screen = registry.ctx().get<ScreenDimensions>();
    const auto& camTransform = registry.get<const TransformComponent>(cameraEntity);
    const float cameraLeft = camTransform.position.x - screen.w / 2.0f;
    const float cameraTop = camTransform.position.y - screen.h / 2.0f;

    // Determine which tiles are visible
    const int startCol = std::max(0, static_cast<int>(std::floor(cameraLeft / tilemap.tileWidth)));
    const int startRow = std::max(0, static_cast<int>(std::floor(cameraTop / tilemap.tileHeight)));
    const int endCol = std::min(tilemap.layers[0].widthInTiles, static_cast<int>(std::floor((cameraLeft + screen.w - 1) / tilemap.tileWidth) + 1));
    const int endRow = std::min(tilemap.layers[0].heightInTiles, static_cast<int>(std::floor((cameraTop + screen.h - 1) / tilemap.tileHeight) + 1));

    SDL_Texture* texture = tileset->textureAtlas.get();
    
    // Draw each visible tile from each layer
    for (const auto& layer : tilemap.layers) {
        for (int row = startRow; row < endRow; ++row) {
            for (int col = startCol; col < endCol; ++col) {
                int tileId = layer.tileIds[row * layer.widthInTiles + col];
                if (tileId == 0) continue; // 0 is an empty tile

                int tileIndex = tileId - 1; // Map IDs are 1-based, array indices are 0-based

                // Calculate source rect from the tileset atlas
                int srcX = (tileIndex % tileset->columns) * tileset->tileWidth;
                int srcY = (tileIndex / tileset->columns) * tileset->tileHeight;
                SDL_Rect srcRect = {srcX, srcY, tileset->tileWidth, tileset->tileHeight};

                // Calculate destination rect using integer coordinates for pixel-perfect drawing.
                SDL_Rect destRect = {
                    static_cast<int>(std::round((col * tilemap.tileWidth) - cameraLeft)),
                    static_cast<int>(std::round((row * tilemap.tileHeight) - cameraTop)),
                    tilemap.tileWidth,
                    tilemap.tileHeight
                };
                SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
            }
        }
    }
}
