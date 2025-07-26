#include "game_scene.hpp"
#include "../util/resource_manager.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/player_control.hpp"
#include "../components/movement.hpp"
#include "../components/intent.hpp"
#include "../components/camera.hpp"
#include "../components/blackboard.hpp"
#include "../core/blackboard_keys.hpp"
#include "../core/context.hpp"
#include <iostream>
#include <vector>

GameScene::GameScene(std::unique_ptr<ISceneLoader> sceneLoader,
    std::string sceneFilePath)
:  m_sceneLoader(std::move(sceneLoader)),
m_sceneFilePath(sceneFilePath)
{}

void GameScene::setSystemManager(std::unique_ptr<SystemManager> systemManager) {
    m_systemManager = std::move(systemManager);
}

void GameScene::load(SDL_Renderer* renderer, ResourceManager* resourceManager,
    InputManager* inputManager, const SceneContext& context) {
    m_resourceManager = resourceManager;
    m_inputManager = inputManager;

    std::cout << "GameScene loading..." << std::endl;
    // Use the loader to populate the registry!
    m_sceneLoader->load(m_registry, renderer, m_resourceManager, m_sceneFilePath);

    // Initialize all systems now that the registry is populated.
    m_systemManager->initAll(m_registry);

    std::cout << "GameScene loaded." << std::endl;
}

SceneContext GameScene::saveState() {
    SceneContext context;

    // Find the player entity and save its position.
    auto view = m_registry.view<const PlayerControlComponent, const TransformComponent>();
    for (auto entity : view) { // Should only be one
        const auto& transform = view.get<const TransformComponent>(entity);
        context["player_position"] = transform.position; // Add position to the context map
        std::cout << "Saved player position to context: " << transform.position.x << ", " << transform.position.y << std::endl;
        break; // Found the player, no need to continue
    }

    return context;
}

void GameScene::unload() {
    std::cout << "GameScene unloading..." << std::endl;
    // In a more complex scenario, you might unload scene-specific assets here
    // to free up memory.
    m_registry.clear();
}

void GameScene::handleEvents(const SDL_Event& event) {
    // Scene-specific event handling would go here.
}

void GameScene::update(float deltaTime) {
    // Delegate to the SystemManager
    m_systemManager->updateAll(m_registry, *m_inputManager, *m_resourceManager, deltaTime);
}

//TODO: is is true that tiles will always be in the background? what about going behind a building in the game?
void GameScene::render(SDL_Renderer* renderer) {
    // Delegate to the SystemManager
    m_systemManager->drawAll(renderer, m_registry, *m_resourceManager);

}
