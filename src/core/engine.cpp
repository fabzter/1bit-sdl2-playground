#include "engine.hpp"
#include <iostream>
#include "scene.hpp"
#include "../scenes/game_scene.hpp" // Include our concrete scene

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

    // Set up the initial scene
    // TODO: make the engine manage scenes in an more general way such that we can organize it without modifying the engine
    m_currentScene = std::make_unique<GameScene>();
    m_currentScene->load(m_renderer.get(), m_resourceManager.get());

    m_lastFrameTime = SDL_GetPerformanceCounter();
    m_isRunning = true;
    return true;
}

void Engine::run() {
    mainLoop();
}

void Engine::mainLoop() {
     while (m_isRunning) {
        handleEvents();
        update();
        render();
    }

    // Unload the current scene before quitting
    if (m_currentScene) {
        m_currentScene->unload();
    }
}

void Engine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
        if (m_currentScene) {
            m_currentScene->handleEvents(event);
        }
    }
}

void Engine::update() {
    uint64_t now = SDL_GetPerformanceCounter();
    float deltaTime = (now - m_lastFrameTime) / static_cast<float>(SDL_GetPerformanceFrequency());
    m_lastFrameTime = now;

    if (m_currentScene) {
        m_currentScene->update(deltaTime);
    }
}

void Engine::render() {
    SDL_SetRenderDrawColor(m_renderer.get(), 15, 15, 15, 255); // A dark grey background
    SDL_RenderClear(m_renderer.get());

    if (m_currentScene) {
        m_currentScene->render(m_renderer.get());
    }

    SDL_RenderPresent(m_renderer.get());
}