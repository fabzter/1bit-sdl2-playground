#include "engine.hpp"
#include <iostream>
#include "scene.hpp"
#include "../scenes/game_scene.hpp"
#include "../util/input_config_loader.hpp"

Engine::Engine() = default;
Engine::~Engine() {
    saveInputBindings();
    m_sceneManager.reset(); // Explicitly reset SceneManager before other managers
    m_inputManager.reset();
    m_resourceManager.reset();
    m_renderer.reset();
    m_window.reset();
    SDL_Quit();
}

void Engine::initUserConfigPath() {
    char* prefPath = SDL_GetPrefPath("fabz", "1bit-playground");
    if (prefPath) {
        m_userConfigPath = std::string(prefPath) + "input.ini";
        SDL_free(prefPath);
    }
}

void Engine::loadInputConfig() {
    // 1. Try loading user's custom config first. It's not an error if it doesn't exist.
    bool loaded = !m_userConfigPath.empty() &&
                  InputConfigLoader::loadFromFile(*m_inputManager, m_userConfigPath);

    if (!loaded) {
        // 2. If that fails, try loading the default config from the resources directory.
        std::string defaultFilePath = m_resourceManager->getBasePath() + "res/config/input.ini";
        loaded = InputConfigLoader::loadFromFile(*m_inputManager, defaultFilePath);
    }

    if (!loaded) {
        // 3. If all file loading fails, use hardcoded defaults as a last resort.
        std::cerr << "Warning: Could not load any input config file. Falling back to hardcoded defaults." << std::endl;
        setupDefaultInputs();
    }
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
    m_inputManager = std::make_unique<InputManager>(8000);

    initUserConfigPath();
    loadInputConfig();

    // SceneManager is created last as it may depend on the others for its scenes.
    m_sceneManager = std::make_unique<SceneManager>(m_renderer.get(),
        m_resourceManager.get(), m_inputManager.get());

    m_lastFrameTime = SDL_GetPerformanceCounter();
    m_isRunning = true;
    return true;
}

void Engine::saveInputBindings() {
    // Don't save if we don't have an input manager.
    if (!m_inputManager) return;

    char* prefPath = SDL_GetPrefPath("Fabz", "1bit-playground");
    if (!prefPath) {
        std::cerr << "Could not save input bindings: unable to find preferences path." << std::endl;
        return;
    }

    // SDL_GetPrefPath ensures the directory exists.
    std::string configFilePath = std::string(prefPath) + "input.ini";
    SDL_free(prefPath);

    InputConfigLoader::saveToFile(*m_inputManager, configFilePath);
}

void Engine::run(const std::string& initialSceneId) {
    m_sceneManager->switchTo(initialSceneId);
    mainLoop();
}

void Engine::mainLoop() {
     while (m_isRunning) {
        handleEvents();
        update();
        render();
    }
}

void Engine::registerScene(const std::string& id, std::unique_ptr<Scene> scene) {
    m_sceneManager->registerScene(id, std::move(scene));
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