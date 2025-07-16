#pragma once

#include <SDL2/SDL.h>

class ResourceManager; // Forward declaration

class Scene {
public:
    virtual ~Scene() = default;

    // Called when the scene is first set as active.
    // Use this to load assets, create entities, etc.
    virtual void load(SDL_Renderer* renderer, ResourceManager* resourceManager) = 0;

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
