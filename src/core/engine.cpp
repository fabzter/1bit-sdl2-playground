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

    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
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

    // Initialize managers and systems
    m_resourceManager = std::make_unique<ResourceManager>();
    m_inputManager = std::make_unique<InputManager>();
    setupDefaultInputs();

    m_playerIntentSystem = std::make_unique<PlayerIntentSystem>();
    m_topDownMovementSystem = std::make_unique<TopDownMovementSystem>();
    m_animationSystem = std::make_unique<AnimationSystem>();
    m_renderSystem = std::make_unique<RenderSystem>();

    // SceneManager is created last as it may depend on the others for its scenes.
    m_sceneManager = std::make_unique<SceneManager>(m_renderer.get(), m_resourceManager.get(), m_inputManager.get());

    registerScenes();
    m_sceneManager->switchTo("game");

    m_lastFrameTime = SDL_GetPerformanceCounter();
    m_isRunning = true;
    return true;
}

void Engine::registerScenes() {
    // The lambda now captures `this` to access the engine's system pointers.
    m_sceneManager->registerScene("game", [this]() {
        // We no longer create systems here. We pass the ones the engine owns.
        return std::make_unique<GameScene>(
            m_playerIntentSystem.get(),
            m_topDownMovementSystem.get(),
            m_animationSystem.get(),
            m_renderSystem.get()
        );
    });
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
}

void Engine::handleEvents() {
    m_inputManager->prepareForUpdate();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
        m_inputManager->handleEvent(event);
        m_sceneManager->handleEvents(event);
    }
}

void Engine::update() {
    uint64_t now = SDL_GetPerformanceCounter();
    float deltaTime = (now - m_lastFrameTime) / static_cast<float>(SDL_GetPerformanceFrequency());
    m_lastFrameTime = now;

    m_sceneManager->update(deltaTime);
}

void Engine::render() {
    SDL_SetRenderDrawColor(m_renderer.get(), 15, 15, 15, 255); // A dark grey background
    SDL_RenderClear(m_renderer.get());

    m_sceneManager->render();

    SDL_RenderPresent(m_renderer.get());
}

void Engine::setupDefaultInputs() {
    // Keyboard
    m_inputManager->mapKeyToAction(SDLK_w, "move_up");
    m_inputManager->mapKeyToAction(SDLK_UP, "move_up");
    m_inputManager->mapKeyToAction(SDLK_s, "move_down");
    m_inputManager->mapKeyToAction(SDLK_DOWN, "move_down");
    m_inputManager->mapKeyToAction(SDLK_a, "move_left");
    m_inputManager->mapKeyToAction(SDLK_LEFT, "move_left");
    m_inputManager->mapKeyToAction(SDLK_d, "move_right");
    m_inputManager->mapKeyToAction(SDLK_RIGHT, "move_right");
    m_inputManager->mapKeyToAction(SDLK_SPACE, "action_button");

    // Controller
    m_inputManager->mapButtonToAction(SDL_CONTROLLER_BUTTON_DPAD_UP, "move_up");
    m_inputManager->mapButtonToAction(SDL_CONTROLLER_BUTTON_DPAD_DOWN, "move_down");
    m_inputManager->mapButtonToAction(SDL_CONTROLLER_BUTTON_DPAD_LEFT, "move_left");
    m_inputManager->mapButtonToAction(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, "move_right");
    m_inputManager->mapButtonToAction(SDL_CONTROLLER_BUTTON_A, "action_button"); // 'A' on Xbox, 'Cross' on PS

    // --- Axis Actions ---
    m_inputManager->mapAxisToAction(SDL_CONTROLLER_AXIS_LEFTX, "move_horizontal");
    m_inputManager->mapAxisToAction(SDL_CONTROLLER_AXIS_LEFTY, "move_vertical");
}