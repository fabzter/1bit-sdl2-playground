#include "tmx_loader.hpp"
#include "../components/tilemap.hpp"
#include "../components/transform.hpp"
#include "../components/collider.hpp"
#include "../util/resource_manager.hpp"
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <iostream>
#include <stdexcept>
#include <sstream>

// A helper to translate layer names to bitmasks.
// This should probably live in a more central "PhysicsConfig" file later.
static uint32_t getLayerBitmask(const std::string& name) {
    if (name == "WORLD") return 1 << 0; // Layer 1
    if (name == "PLAYER") return 1 << 1; // Layer 2
    // Add other layers here...
    return 0;
}

bool TmxLoader::load(entt::registry& registry, entt::entity tilemapEntity,
    ResourceManager& resourceManager, const std::string& sourcePath) {
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

        // parse object layers
        else if (layer->getType() == tmx::Layer::Type::Object) {
            if (layer->getName() == "Collisions") {
                const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();
                for (const auto& object : objectLayer.getObjects()) {
                    // Create a new entity for each collision object
                    const auto entity = registry.create();

                    // Add a TransformComponent based on the object's position and size
                    const auto& aabb = object.getAABB();
                    registry.emplace<TransformComponent>(entity,
                        Vec2f{aabb.left + aabb.width / 2.f, aabb.top + aabb.height / 2.f}, // Position is center
                        Vec2f{1.f, 1.f}
                    );

                    // Add a ColliderComponent
                    auto& collider = registry.emplace<ColliderComponent>(entity);
                    collider.size = {aabb.width, aabb.height};
                    collider.is_static = true;

                    uint32_t final_mask = 0;
                    for (const auto& prop : object.getProperties()) {
                        if (prop.getName() == "collisionLayerName") {
                            collider.layer = getLayerBitmask(prop.getStringValue());
                        } else if (prop.getName() == "collisionMaskNames") {
                            std::string maskStr = prop.getStringValue();
                            std::stringstream ss(maskStr);
                            std::string name;
                            while(std::getline(ss, name, ',')) {
                                final_mask |= getLayerBitmask(name);
                            }
                        }
                    }
                    collider.mask = final_mask;
                }
            }
        }
    }
    std::cout << "TmxLoader: Successfully loaded map '" << sourcePath << "'" << std::endl;
    return true;
}
