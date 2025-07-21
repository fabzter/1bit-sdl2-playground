#include "scene_manager.hpp"
#include <iostream>

SceneManager::SceneManager(SDL_Renderer* renderer, ResourceManager* resourceManager,
    InputManager* inputManager)
    : m_renderer(renderer), m_resourceManager(resourceManager), m_inputManager(inputManager) {}

SceneManager::~SceneManager() {
    shutdown();
}

void SceneManager::registerScene(const std::string& id, std::function<std::unique_ptr<Scene>()> factory) {
    m_sceneFactories[id] = std::move(factory);
    std::cout << "SceneManager: Registered scene with ID '" << id << "'." << std::endl;
}

void SceneManager::switchTo(const std::string& id) {
    auto it = m_sceneFactories.find(id);
    if (it == m_sceneFactories.end()) {
        std::cerr << "SceneManager: No scene registered with ID '" << id << "'." << std::endl;
        return;
    }

    // create a context before transition
    SceneContext context;

    // Unload the previous scene if it exists
    if (m_currentScene) {
        context = m_currentScene->saveState(); // Save the current scene's state
        m_currentScene->unload();
    }

    // Create and load the new scene using its factory
    m_currentScene = it->second();
    std::cout << "SceneManager: Switching to scene '" << id << "'." << std::endl;
    m_currentScene->load(m_renderer, m_resourceManager, m_inputManager, context);
}

void SceneManager::handleEvents(const SDL_Event& event) {
    if (m_currentScene) {
        m_currentScene->handleEvents(event);
    }
}

void SceneManager::update(float deltaTime) {
    if (m_currentScene) {
        m_currentScene->update(deltaTime);
    }
}

void SceneManager::render() {
    if (m_currentScene) {
        m_currentScene->render(m_renderer);
    }
}

void SceneManager::shutdown() {
    if (m_currentScene) {
        m_currentScene->unload();
        m_currentScene.reset();
    }
}