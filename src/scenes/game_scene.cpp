#include "game_scene.hpp"
#include "../util/resource_manager.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/player_control.hpp"
#include "../components/movement.hpp"
#include <iostream>
#include <vector>

void GameScene::load(SDL_Renderer* renderer, ResourceManager* resourceManager,
                     InputManager* inputManager) {
    m_resourceManager = resourceManager;
    m_inputManager = inputManager;

    m_renderSystem = std::make_unique<RenderSystem>();
    m_animationSystem = std::make_unique<AnimationSystem>();
    m_playerControlSystem = std::make_unique<PlayerControlSystem>();

    std::cout << "GameScene loading..." << std::endl;
    std::vector<std::string> assetsToPreload = {
        "player"
        // Any other assets for this scene would go here.
    };

    // This is our explicit preload step for the scene!
    m_resourceManager->preloadSpriteAssets(renderer, assetsToPreload);

    // --- Create Player Entity ---
    const auto player = m_registry.create();

    // tag player as controllable
    m_registry.emplace<PlayerControlComponent>(player);

    // Add the new MovementComponent with a starting speed
    m_registry.emplace<MovementComponent>(player, 200.0f);

    // 1. Get the loaded sprite asset from the resource manager
    const SpriteAsset* playerAsset = m_resourceManager->getSpriteAsset(renderer, "player");

    if (playerAsset) {
        // 2. Use the asset's data to construct our components
        m_registry.emplace<TransformComponent>(player, Vec2f{0.0f, 0.0f}, Vec2f{4.0f, 4.0f});
        auto& sprite = m_registry.emplace<SpriteComponent>(player, playerAsset->assetId, playerAsset->width, playerAsset->height);
        sprite.isAnimated = true; //TODO: probably should activate automatically depending on sprite data
    }
    // --- End Player Entity ---

    std::cout << "GameScene loaded." << std::endl;
}

void GameScene::unload() {
    std::cout << "GameScene unloading..." << std::endl;
    // In a more complex scenario, you might unload scene-specific assets here
    // to free up memory.
    m_registry.clear(); // clear entities and components
    m_renderSystem.reset();
}

void GameScene::handleEvents(const SDL_Event& event) {
    // Scene-specific event handling would go here.
}

void GameScene::update(float deltaTime) {
    m_playerControlSystem->update(m_registry, *m_inputManager, deltaTime);
    m_animationSystem->update(m_registry, deltaTime, *m_resourceManager);
}

void GameScene::render(SDL_Renderer* renderer) {
    m_renderSystem->draw(renderer, m_registry, *m_resourceManager);
}
