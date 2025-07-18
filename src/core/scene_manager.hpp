#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <SDL2/SDL.h>

#include "scene.hpp"

class SceneManager {
public:
    SceneManager(SDL_Renderer* renderer, ResourceManager* resourceManager);
    ~SceneManager();

    // Disallow copying
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    /**
     * @brief Registers a scene factory with a unique ID.
     * The factory is a function that returns a new instance of a scene.
     * @param id The unique string identifier for the scene (e.g., "game", "menu").
     * @param factory The lambda or function that creates the scene.
     */
    void registerScene(const std::string& id, std::function<std::unique_ptr<Scene>()> factory);

    /**
     * @brief Unloads the current scene and loads/switches to a new one.
     * @param id The ID of the scene to switch to.
     */
    void switchTo(const std::string& id);

    // --- Engine Call Delegation ---
    void handleEvents(const SDL_Event& event);
    void update(float deltaTime);
    void render();

    /**
     * @brief Unloads the current scene. Called before the engine shuts down.
     */
    void shutdown();

private:
    // Non-owning pointers to the engine's core resources
    SDL_Renderer* m_renderer;
    ResourceManager* m_resourceManager;

    std::unordered_map<std::string, std::function<std::unique_ptr<Scene>()>> m_sceneFactories;
    std::unique_ptr<Scene> m_currentScene;
};