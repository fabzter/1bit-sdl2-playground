#include "hardcoded_map_loader.hpp"
#include "../components/tilemap.hpp"
#include "../components/transform.hpp"
#include "../components/collider.hpp"
#include "../components/rigidbody.hpp"
#include "../util/resource_manager.hpp"
#include <iostream>

// Helper to translate layer names to bitmasks.
static uint32_t getLayerBitmask(const std::string& name) {
    if (name == "WORLD") return 1 << 0;
    if (name == "PLAYER") return 1 << 1;
    return 0;
}

HardcodedMapLoader::HardcodedMapLoader(const TMX::Map& mapDescriptor)
    : m_mapDescriptor(mapDescriptor) {}

bool HardcodedMapLoader::load(entt::registry& registry, entt::entity tilemapEntity,
                              ResourceManager& resourceManager, const std::string&) {

    // 1. Load Tileset Assets
    // This step ensures the textures for our tilesets are loaded into the ResourceManager.
    for (const auto& tilesetDesc : m_mapDescriptor.tilesets) {
        if (tilesetDesc.image) {
            // The source hint (e.g., "ground.tileset") tells the ResourceManager to use our custom loader.
            resourceManager.loadTilesetAsset(registry.ctx().get<SDL_Renderer*>(), tilesetDesc.name, tilesetDesc.image->source);
        }
    }
    
    // For now, we assume the first tileset is the primary one for the map.
    const auto& firstTileset = m_mapDescriptor.tilesets[0];
    
    // 2. Create the main TilemapComponent
    auto& tilemap = registry.emplace<TilemapComponent>(tilemapEntity);
    tilemap.tileWidth = firstTileset.tileWidth;
    tilemap.tileHeight = firstTileset.tileHeight;
    tilemap.tilesetAssetId = firstTileset.name;

    // 3. Process Layers
    for (const auto& layerVariant : m_mapDescriptor.layers) {
        // Use std::visit to handle the different layer types in the variant
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, TMX::TileLayer>) {
                // This is a tile layer, so we add its data to our TilemapComponent.
                auto& newLayer = tilemap.layers.emplace_back();
                newLayer.widthInTiles = arg.width;
                newLayer.heightInTiles = arg.height;
                // The payload is also a variant, we assume it's the vector of GIDs.
                if (arg.data) {
                    if (const auto* gids = std::get_if<std::vector<uint32_t>>(&arg.data->payload)) {
                        newLayer.tileIds.assign(gids->begin(), gids->end());
                    }
                }
            }
            else if constexpr (std::is_same_v<T, TMX::ObjectGroup>) {
                // This is an object group. Check if it's our collision layer.
                if (arg.name == "Collisions") {
                    createCollisionObjects(registry, arg);
                }
            }
            else if constexpr (std::is_same_v<T, TMX::ImageLayer>) {
               // TODO: Handle ImageLayer
           }
           else if constexpr (std::is_same_v<T, std::unique_ptr<TMX::GroupLayer>>) {
               // TODO: Handle GroupLayer
           }
        }, layerVariant);
    }
    
    std::cout << "HardcodedMapLoader: Successfully processed map descriptor." << std::endl;
    return true;
}

void HardcodedMapLoader::createCollisionObjects(entt::registry& registry, const TMX::ObjectGroup& objectGroup) {
    for (const auto& objectDesc : objectGroup.objects) {
        const auto entity = registry.create();

        // Position is the center of the AABB.
        registry.emplace<TransformComponent>(entity,
        Vec2f{objectDesc.position.x + objectDesc.size.x / 2.f, objectDesc.position.y + objectDesc.size.y / 2.f}
        );

        auto& collider = registry.emplace<ColliderComponent>(entity);
        collider.size = {objectDesc.size.x, objectDesc.size.y};
        collider.is_static = true;

        if (objectDesc.properties) {
            uint32_t mask = 0;
            for (const auto& prop : objectDesc.properties->list) {
                if (prop.name == "collisionLayerName") {
                    collider.layer = getLayerBitmask(std::get<std::string>(prop.value));
                } else if (prop.name == "collisionMaskNames") {
                    std::string maskStr = std::get<std::string>(prop.value);
                    // This could be extended to handle comma-separated values if needed.
                    mask |= getLayerBitmask(maskStr);
                }
            }
            collider.mask = mask;
        }
        
        // Add a static rigidbody to all collision objects by default.
        registry.emplace<RigidBodyComponent>(entity, RigidBodyComponent{.bodyType = BodyType::STATIC, .mass = 0.0f});
    }
}
