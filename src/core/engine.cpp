#include "engine.hpp"
#include <iostream>

Engine::Engine() = default;
Engine::~Engine() {
    SDL_Quit();
}

bool Engine::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Initialization Error: " << SDL_GetError() << std::endl;
        return false;
    }

    m_window.reset(SDL_CreateWindow(
        "1-Bit Playground",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    ));

    if (!m_window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    m_renderer.reset(SDL_CreateRenderer(
        m_window.get(),
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    ));

    if (!m_renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    m_resourceManager = std::make_unique<ResourceManager>();

    std::vector<std::string> assetsToPreload = {
        "player"
        // "enemy_ship", "bullet", "explosion", etc.
    };

    // 4. Preload all required assets at once
    m_resourceManager->preloadTextures(m_renderer.get(), assetsToPreload);
    m_isRunning = true;
    return true;
}

void Engine::run() {
    while (m_isRunning) {
        handleEvents();
        update();
        render();
    }
}

void Engine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
    }
}

void Engine::update() {
    // Game logic updates will go here
}

void Engine::render() {
    SDL_SetRenderDrawColor(m_renderer.get(), 15, 15, 15, 255); // A dark grey background
    SDL_RenderClear(m_renderer.get());

    // All drawing commands will be called from the render system
    m_renderSystem->draw(m_renderer.get(), *m_resourceManager);

    SDL_RenderPresent(m_renderer.get());
}