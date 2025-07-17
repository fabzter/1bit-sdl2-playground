#include "game_scene.hpp"
#include "../util/resource_manager.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include <iostream>
#include <vector>

void GameScene::load(SDL_Renderer* renderer, ResourceManager* resourceManager) {
    m_resourceManager = resourceManager;
    m_renderSystem = std::make_unique<RenderSystem>();

    std::cout << "GameScene loading..." << std::endl;
    std::vector<std::string> assetsToPreload = {
        "player"
        // Any other assets for this scene would go here.
    };

    // This is our explicit preload step for the scene!
    m_resourceManager->preloadSpriteAssets(renderer, assetsToPreload);

    // --- Create Player Entity ---
    const auto player = m_registry.create();

    // 1. Get the loaded sprite asset from the resource manager
    const SpriteAsset* playerAsset = m_resourceManager->getSpriteAsset(renderer, "player");

    if (playerAsset) {
        // 2. Use the asset's data to construct our components
        m_registry.emplace<TransformComponent>(player, Vec2f{0.0f, 0.0f}, Vec2f{1.0f, 1.0f});
        m_registry.emplace<SpriteComponent>(player, playerAsset->assetId, playerAsset->width, playerAsset->height);
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
    // The `Engine::update` is now empty, so game logic updates for this scene go here.
}

void GameScene::render(SDL_Renderer* renderer) {
    m_renderSystem->draw(renderer, m_registry, *m_resourceManager);
}
