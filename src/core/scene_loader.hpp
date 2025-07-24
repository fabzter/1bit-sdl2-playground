#pragma once

#include <string>
#include <entt/entt.hpp>
#include <SDL2/SDL.h>

// Forward-declarations to avoid including heavy headers
class ResourceManager;

/**
 * @class ISceneLoader
 * @brief An interface for classes that load scene data into an ECS registry.
 */
class ISceneLoader {
public:
    virtual ~ISceneLoader() = default;

    /**
     * @brief Loads scene data from a specified source path.
     * @param registry The registry to populate.
     * @param renderer A pointer to the SDL renderer.
     * @param resourceManager A pointer to the resource manager for assets.
     * @param sourcePath The path to the scene file.
     * @return True if loading was successful, false otherwise.
     */
    virtual bool load(entt::registry& registry,
        SDL_Renderer* renderer,
        ResourceManager* resourceManager,
        const std::string& sourcePath) = 0;
};
