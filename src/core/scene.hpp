#pragma once

#include <SDL2/SDL.h>
#include "input_manager.hpp"
#include "scene_context.hpp"

class ResourceManager; // Forward declaration

class Scene {
public:
    virtual ~Scene() = default;

    // Called when the scene is first set as active.
    // Use this to load assets, create entities, etc.
    virtual void load(SDL_Renderer* renderer, ResourceManager* resourceManager, InputManager* inputManager,
        const SceneContext& context) = 0;

    // A method for saving state before the scene is unloaded
    virtual SceneContext saveState() { return {}; } // Default implementation returns an empty context

    // Called when the scene is being deactivated.
    // Use this to clean up resources specific to this scene.
    virtual void unload() = 0;

    // Handle events specific to this scene
    virtual void handleEvents(const SDL_Event& event) = 0;

    // Called every frame for game logic updates.
    virtual void update(float deltaTime) = 0;

    // Called every frame to draw the scene.
    virtual void render(SDL_Renderer* renderer) = 0;
};
