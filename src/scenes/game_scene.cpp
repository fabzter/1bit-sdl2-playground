#include "game_scene.hpp"
#include "../util/resource_manager.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/player_control.hpp"
#include "../components/movement.hpp"
#include "../components/intent.hpp"
#include "../components/camera.hpp"
#include "../core/context.hpp"
#include <iostream>
#include <vector>

GameScene::GameScene(
    std::unique_ptr<PlayerIntentSystem> playerIntentSystem,
    std::unique_ptr<TopDownMovementSystem> topDownMovementSystem,
    std::unique_ptr<AnimationSystem> animationSystem,
    std::unique_ptr<RenderSystem> renderSystem,
    std::unique_ptr<CameraSystem> cameraSystem)
    : m_playerIntentSystem(std::move(playerIntentSystem)),
      m_topDownMovementSystem(std::move(topDownMovementSystem)),
      m_animationSystem(std::move(animationSystem)),
      m_renderSystem(std::move(renderSystem)),
      m_cameraSystem(std::move(cameraSystem))

{}

void GameScene::load(SDL_Renderer* renderer, ResourceManager* resourceManager,
                     InputManager* inputManager, const SceneContext& context) {
    m_resourceManager = resourceManager;
    m_inputManager = inputManager;

    // Connect EnTT signals for automatic camera caching
    m_registry.on_construct<CameraComponent>().connect<&GameScene::onCameraConstruct>(this);
    m_registry.on_destroy<CameraComponent>().connect<&GameScene::onCameraDestroy>(this);

    std::cout << "GameScene loading..." << std::endl;
    std::vector<std::string> assetsToPreload = {
        "player"
        // Any other assets for this scene would go here.
    };
    // This is our explicit preload step for the scene!
    m_resourceManager->preloadSpriteAssets(renderer, assetsToPreload);

    // --- Populate registry context
    // set screen dimensions (could be updated by engine on window resize)
    int screenW, screenH;
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
    m_registry.ctx().emplace<ScreenDimensions>(static_cast<float>(screenW), static_cast<float>(screenH));
    // Set world boundaries for this specific scene
    //TODO: this is severely hardcoded
    m_registry.ctx().emplace<WorldBounds>(SDL_FRect{ -320.0f, -180.0f, 1920.0f, 1080.0f });

    // --- Create Player Entity ---
    const auto player = m_registry.create();
    // tag player as controllable
    m_registry.emplace<PlayerControlComponent>(player);
    // Add the new MovementComponent with a starting speed
    m_registry.emplace<MovementComponent>(player, 200.0f); //TODO: is this the right place to set player speed?
    m_registry.emplace<IntentComponent>(player);

    // Place the player somewhere inside the world bounds
    const auto& bounds = m_registry.ctx().get<WorldBounds>().rect;
    Vec2f startingPosition = { bounds.x + bounds.w / 2.0f, bounds.y + bounds.h / 2.0f };

    // example on how to load data from the context
    if (const auto it = context.find("startingPosition"); it != context.end()) {
        try {
            startingPosition = std::any_cast<Vec2f>(it->second);
            std::cout << "Loaded player position from context: " << startingPosition.x << ", " <<
                startingPosition.y << std::endl;
        } catch (const std::bad_any_cast& e) {
            std::cerr << "Failed to cast player_position: " << e.what() << std::endl;
        }
    }

    // 1. Get the loaded sprite asset from the resource manager
    const SpriteAsset* playerAsset = m_resourceManager->getSpriteAsset(renderer, "player");
    if (playerAsset) {
        // 2. Use the asset's data to construct our components
        m_registry.emplace<TransformComponent>(player, startingPosition, Vec2f{2.0f, 2.0f});
        auto& sprite = m_registry.emplace<SpriteComponent>(player, playerAsset->assetId, playerAsset->width, playerAsset->height);
        sprite.isAnimated = true; //TODO: probably should activate automatically depending on sprite data
    }
    // --- End Player Entity ---

    // --- Create Camera Entity ---
    const auto camera = m_registry.create();
    m_registry.emplace<TransformComponent>(camera); // camera needs a positions in the world
    auto& camComponent = m_registry.emplace<CameraComponent>(camera);
    camComponent.target = player; // tell the camera to follow the player. This will trigger onCameraConstruct

    // Immediately set the camera's position to the player's starting position.
    const auto& playerTransform = m_registry.get<TransformComponent>(player);
    auto& cameraTransform = m_registry.get<TransformComponent>(camera);
    cameraTransform.position = playerTransform.position;

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

    m_animationSystem->update(m_registry, deltaTime, *m_resourceManager);
}

void GameScene::render(SDL_Renderer* renderer) {
    // Note: resource manager is still passed directly here, which is fine.
    m_renderSystem->draw(renderer, m_registry, *m_resourceManager);
}

// --- Signal Handlers ---

void GameScene::onCameraConstruct(entt::registry& registry, entt::entity entity) {
    const auto& camera = registry.get<CameraComponent>(entity);
    if (camera.isActive) {
        // Set this camera as the active one in the context.
        // This will overwrite any previous active camera.
        registry.ctx().emplace<ActiveCamera>(entity);
    }
}

void GameScene::onCameraDestroy(entt::registry& registry, entt::entity entity) {
    // If the camera being destroyed is the active one, remove it from the context.
    const auto& activeCamera = registry.ctx().get<ActiveCamera>();
    if (activeCamera.entity == entity) {
        registry.ctx().erase<ActiveCamera>();
    }
}
