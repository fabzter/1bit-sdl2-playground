#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>

// Forward-declare to avoid including heavy headers in the interface
class ResourceManager;
class InputManager;

/**
 * @brief Base interface for any system that runs logic in the main update loop.
 */
class IUpdateSystem {
public:
    virtual ~IUpdateSystem() = default;
    virtual void update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) = 0;
};

/**
 * @brief Base interface for any system that draws to the screen.
 */
class IRenderSystem {
public:
    virtual ~IRenderSystem() = default;
    virtual void draw(SDL_Renderer* renderer, entt::registry& registry,
        ResourceManager& resourceManager) = 0;
};
