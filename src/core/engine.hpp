#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "../util/resource_manager.hpp"
#include "scene_manager.hpp"
#include "input_manager.hpp"

// Custom deleters for SDL resources to use with smart pointers
struct SDL_Deleter {
    void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
    void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
};

class Scene; // Forward-declaration

class Engine {
public:
    Engine();
    ~Engine();

    bool init();
    void run(const std::string& initialSceneId);

    [[nodiscard]]
    SDL_Renderer* getRenderer() const { return m_renderer.get(); }
    ResourceManager* getResourceManager() { return m_resourceManager.get(); }
    SceneManager* getSceneManager() { return m_sceneManager.get(); }

    // This allows the user to add their own scene factories
    void registerSceneFactory(const std::string& id, std::function<std::unique_ptr<Scene>()> factory);

private:
    void handleEvents();
    void update();
    void render();
    void mainLoop();
    void setupDefaultInputs();

    // --- Member Declaration Order Matters for Destruction! ---
    // The C++ compiler will destruct these in reverse order of declaration.
    // Dependencies must be declared before the objects that use them.

    // 1. Core SDL objects (destructed last)
    std::unique_ptr<SDL_Window, SDL_Deleter> m_window;
    std::unique_ptr<SDL_Renderer, SDL_Deleter> m_renderer;

    // 2. Managers (depend on core SDL objects)
    std::unique_ptr<ResourceManager> m_resourceManager;
    std::unique_ptr<InputManager> m_inputManager;

    // 3. Scene Manager (depends on systems and managers, must be destructed first)
    std::unique_ptr<SceneManager> m_sceneManager;

    // --- State Variables ---
    bool m_isRunning = false;
    uint64_t m_lastFrameTime = 0;
};