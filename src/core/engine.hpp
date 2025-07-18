#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "../util/resource_manager.hpp"
#include "scene_manager.hpp"

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
    void run();

    SDL_Renderer* getRenderer() const { return m_renderer.get(); }
    ResourceManager* getResourceManager() { return m_resourceManager.get(); }

private:
    void handleEvents();
    void update();
    void render();
    void mainLoop();
    void registerScenes();

    bool m_isRunning = false;
    std::unique_ptr<SDL_Window, SDL_Deleter> m_window;
    std::unique_ptr<SDL_Renderer, SDL_Deleter> m_renderer;
    
    std::unique_ptr<ResourceManager> m_resourceManager;
    std::unique_ptr<SceneManager> m_sceneManager;

    uint64_t m_lastFrameTime = 0;
};