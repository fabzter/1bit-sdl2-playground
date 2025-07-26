#include "debug_info_system.hpp"
#include "../components/tilemap.hpp"
#include <iostream>

void DebugInfoSystem::update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) {
    if (inputManager.isActionJustPressed("dump_debug_info")) {
        auto mapView = registry.view<TilemapComponent>();
        if (mapView.empty()) {
            std::cout << "[DEBUG DUMP] No TilemapComponent found in scene." << std::endl;
            return;
        }

        // Get the tilemap component and its first layer
        const auto& tilemap = mapView.get<TilemapComponent>(mapView.front());
        if (tilemap.layers.empty()) {
            std::cout << "[DEBUG DUMP] TilemapComponent found, but its 'layers' vector is empty." << std::endl;
            return;
        }
        const auto& layer = tilemap.layers[0];

        // Get the associated tileset asset
        const TilesetAsset* tileset = resourceManager.getTilesetAsset(tilemap.tilesetAssetId);
        if (!tileset) {
            std::cout << "[DEBUG DUMP] TilesetAsset '" << tilemap.tilesetAssetId << "' not found." << std::endl;
            return;
        }

        // --- DUMP ALL RELEVANT DATA ---
        std::cout << "\n\n==================== DEBUG DUMP ====================" << std::endl;
        std::cout << "--- TilemapComponent State ---" << std::endl;
        std::cout << "  Layer Dimensions: " << layer.widthInTiles << "x" << layer.heightInTiles << std::endl;
        std::cout << "  Tile Data Size:   " << layer.tileIds.size() << " tiles" << std::endl;
        std::cout << "  Expected Size:    " << (layer.widthInTiles * layer.heightInTiles) << " tiles" << std::endl;
        
        std::cout << "\n--- TilesetAsset State ---" << std::endl;
        std::cout << "  Asset ID:      " << tileset->assetId << std::endl;
        std::cout << "  Tile Size:     " << tileset->tileWidth << "x" << tileset->tileHeight << std::endl;
        std::cout << "  Columns:       " << tileset->columns << std::endl;
        std::cout << "  Tile Count:    " << tileset->tileCount << std::endl;

        // Query the actual texture to verify its properties
        if (tileset->textureAtlas) {
            int textureW, textureH;
            SDL_QueryTexture(tileset->textureAtlas.get(), nullptr, nullptr, &textureW, &textureH);
            std::cout << "  Texture Atlas Size: " << textureW << "x" << textureH << " pixels" << std::endl;
        } else {
            std::cout << "  Texture Atlas:  (null)" << std::endl;
        }
        std::cout << "====================================================\n\n" << std::endl;
    }
}
