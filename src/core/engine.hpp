#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "../systems/renderer.hpp"
#include "../util/resource_manager.hpp" 

// Custom deleters for SDL resources to use with smart pointers
struct SDL_Deleter {
    void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
    void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
};

class Engine {
public:
    Engine();
    ~Engine();

    bool init();
    void run();

private:
    void handleEvents();
    void update();
    void render();

    bool m_isRunning = false;
    std::unique_ptr<SDL_Window, SDL_Deleter> m_window;
    std::unique_ptr<SDL_Renderer, SDL_Deleter> m_renderer;
    
    // Systems
    std::unique_ptr<ResourceManager> m_resourceManager;
    std::unique_ptr<RenderSystem> m_renderSystem;
};