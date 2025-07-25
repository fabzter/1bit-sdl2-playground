#pragma once

#include <string>
#include <entt/entt.hpp>

// Forward-declarations
class ResourceManager;

/**
 * @class ITilemapLoader
 * @brief An interface for classes that load tilemap data into an entity.
 */
class ITilemapLoader {
public:
    virtual ~ITilemapLoader() = default;

    /**
     * @brief Loads map data from a source and configures the entity's TilemapComponent.
     * @param registry The scene's ECS registry.
     * @param tilemapEntity The entity that will hold the tilemap.
     * @param resourceManager The engine's resource manager.
     * @param sourcePath The path to the map file (e.g., a .tmx file).
     * @return True if loading was successful, false otherwise.
     */
    virtual bool load(entt::registry& registry,
                      entt::entity tilemapEntity,
                      ResourceManager& resourceManager,
                      const std::string& sourcePath) = 0;
};
