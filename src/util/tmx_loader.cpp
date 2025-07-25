#include "tmx_loader.hpp"
#include "../components/tilemap.hpp"
#include "../util/resource_manager.hpp"
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <iostream>
#include <stdexcept>

bool TmxLoader::load(entt::registry& registry, entt::entity tilemapEntity, ResourceManager& resourceManager, const std::string& sourcePath) {
    tmx::Map map;
    if (!map.load(sourcePath)) {
        std::cerr << "TmxLoader: Failed to load map file: " << sourcePath << std::endl;
        return false;
    }

    // Load the tileset assets required by the map
    for (const auto& tileset : map.getTilesets()) {
        // We use the tileset name from Tiled as the assetId for our ResourceManager
        resourceManager.loadTilesetAsset(registry.ctx().get<SDL_Renderer*>(), tileset.getName());
    }

    // We assume the first tileset is the one we want to use for the whole map
    // A more complex implementation could support multiple tilesets.
    const auto& firstTileset = map.getTilesets()[0];
    
    auto& tilemap = registry.emplace<TilemapComponent>(tilemapEntity);
    tilemap.tileWidth = firstTileset.getTileSize().x;
    tilemap.tileHeight = firstTileset.getTileSize().y;
    tilemap.tilesetAssetId = firstTileset.getName();

    // Process each layer in the map
    for (const auto& layer : map.getLayers()) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
            
            TileLayer currentLayer;
            currentLayer.widthInTiles = tileLayer->getSize().x;
            currentLayer.heightInTiles = tileLayer->getSize().y;

            // ---- VALIDATION CHECK ----
            const auto& tiles = tileLayer->getTiles();
            size_t expectedTileCount = currentLayer.widthInTiles * currentLayer.heightInTiles;

            if (tiles.size() != expectedTileCount) {
                std::string errorMsg = "TmxLoader Error: Tile data mismatch in '" + sourcePath + "'. "
                                     + "Layer '" + layer->getName() + "' declares dimensions "
                                     + std::to_string(currentLayer.widthInTiles) + "x" + std::to_string(currentLayer.heightInTiles)
                                     + " (expected " + std::to_string(expectedTileCount) + " tiles), "
                                     + "but contains data for " + std::to_string(tiles.size()) + " tiles.";
                throw std::runtime_error(errorMsg);
            }
            // ------------------------

            currentLayer.tileIds.reserve(tileLayer->getTiles().size());
            for(const auto& tile : tileLayer->getTiles()) {
                // The tile.ID is the Global ID (GID) from the tileset.
                // Tiled uses 0 for empty tiles, so this maps directly to our system.
                currentLayer.tileIds.push_back(tile.ID);
            }
            tilemap.layers.push_back(std::move(currentLayer));
        }
    }
    std::cout << "TmxLoader: Successfully loaded map '" << sourcePath << "'" << std::endl;
    return true;
}
