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

GameScene::GameScene(
    std::unique_ptr<ISceneLoader> sceneLoader,
    const std::string& sceneFilePath,
    std::unique_ptr<PlayerIntentSystem> playerIntentSystem,
    std::unique_ptr<TopDownMovementSystem> topDownMovementSystem,
    std::unique_ptr<AnimationStateSystem> animationStateSystem,
    std::unique_ptr<AnimationSystem> animationSystem,
    std::unique_ptr<RenderSystem> renderSystem,
    std::unique_ptr<TilemapRenderSystem> tilemapRenderSystem,
    std::unique_ptr<CameraSystem> cameraSystem)
:  m_sceneLoader(std::move(sceneLoader)),
m_sceneFilePath(sceneFilePath),
m_playerIntentSystem(std::move(playerIntentSystem)),
m_topDownMovementSystem(std::move(topDownMovementSystem)),
m_animationStateSystem(std::move(animationStateSystem)),
m_animationSystem(std::move(animationSystem)),
m_renderSystem(std::move(renderSystem)),
m_tilemapRenderSystem(std::move(tilemapRenderSystem)),
m_cameraSystem(std::move(cameraSystem))

{}

void GameScene::load(SDL_Renderer* renderer, ResourceManager* resourceManager,
                     InputManager* inputManager, const SceneContext& context) {
    m_resourceManager = resourceManager;
    m_inputManager = inputManager;

    std::cout << "GameScene loading..." << std::endl;
    // Use the loader to populate the registry!
    m_sceneLoader->load(m_registry, renderer, m_resourceManager, m_sceneFilePath);

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
    // Update systems in logical order
    m_cameraSystem->update(m_registry, deltaTime);
    m_playerIntentSystem->update(m_registry, *m_inputManager, deltaTime); // 1. Populate intent
    m_topDownMovementSystem->update(m_registry, deltaTime); // 2. Apply movement from intent
    m_animationStateSystem->update(m_registry);
    m_animationSystem->update(m_registry,  deltaTime, *m_resourceManager);
}

//TODO: is is true that tiles will always be in the background? what about going behind a building in the game?
void GameScene::render(SDL_Renderer* renderer) {
    // Draw the tilemap FIRST (in the background)
    m_tilemapRenderSystem->draw(renderer, m_registry, *m_resourceManager);
    // Draw sprites and other objects on top
    // Note: resource manager is still passed directly here, which is fine.
    m_renderSystem->draw(renderer, m_registry, *m_resourceManager);
}
