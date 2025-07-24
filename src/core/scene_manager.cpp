#include "scene_manager.hpp"
#include <iostream>

SceneManager::SceneManager(SDL_Renderer* renderer, ResourceManager* resourceManager,
    InputManager* inputManager)
    : m_renderer(renderer), m_resourceManager(resourceManager), m_inputManager(inputManager) {}

SceneManager::~SceneManager() {
    shutdown();
}

void SceneManager::registerScene(const std::string& id, std::unique_ptr<Scene> scene) {
    m_scenes[id] = std::move(scene);
    std::cout << "SceneManager: Registered scene with ID '" << id << "'." << std::endl;
}

void SceneManager::switchTo(const std::string& id) {
    auto it = m_scenes.find(id);
    if (it == m_scenes.end()) {
        std::cerr << "SceneManager: No scene registered with ID '" << id << "'." << std::endl;
        return;
    }

    // create a context before transition
    SceneContext context;

    // Unload the previous scene if it exists
    if (!m_currentSceneId.empty() && m_scenes.count(m_currentSceneId)) {
        m_scenes[m_currentSceneId]->unload();
    }

    // Load the new scene
    m_currentSceneId = id;
    std::cout << "SceneManager: Switching to scene '" << id << "'." << std::endl;
    m_scenes[id]->load(m_renderer, m_resourceManager, m_inputManager, context);
}

void SceneManager::handleEvents(const SDL_Event& event) {
    if (!m_currentSceneId.empty()) {
        m_scenes[m_currentSceneId]->handleEvents(event);
    }
}

void SceneManager::update(float deltaTime) {
    if (!m_currentSceneId.empty()) {
        m_scenes[m_currentSceneId]->update(deltaTime);
    }
}

void SceneManager::render() {
    if (!m_currentSceneId.empty()) {
        m_scenes[m_currentSceneId]->render(m_renderer);
    }
}

void SceneManager::shutdown() {
    if (!m_currentSceneId.empty() && m_scenes.count(m_currentSceneId)) {
        m_scenes[m_currentSceneId]->unload();
    }
    m_scenes.clear();
}