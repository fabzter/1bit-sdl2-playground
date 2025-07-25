#pragma once

#include <memory>
#include "../core/scene.hpp"
#include "../core/scene_loader.hpp"
#include "../systems/renderer.hpp"
#include "../systems/animation.hpp"
#include "../systems/player_intent_system.hpp"
#include "../systems/top_down_movement_system.hpp"
#include "../systems/animation_state_system.hpp"
#include "../systems/camera_system.hpp"
#include "../systems/tilemap_render_system.hpp"

// Forward-declare systems and managers
class RenderSystem;
class AnimationSystem;
class PlayerIntentSystem;
class TopDownMovementSystem;
class CameraSystem;
class AnimationStateSystem;

class ResourceManager;
class InputManager;

class GameScene : public Scene {
public:
    GameScene(
        std::unique_ptr<ISceneLoader> sceneLoader,
        const std::string& sceneFilePath,
        std::unique_ptr<PlayerIntentSystem> playerIntentSystem,
        std::unique_ptr<TopDownMovementSystem> topDownMovementSystem,
        std::unique_ptr<AnimationStateSystem> animationStateSystem,
        std::unique_ptr<AnimationSystem> animationSystem,
        std::unique_ptr<RenderSystem> renderSystem,
        std::unique_ptr<TilemapRenderSystem> tilemapRenderSystem,
        std::unique_ptr<CameraSystem> cameraSystem
    );
    ~GameScene() override = default;

    void load(SDL_Renderer* renderer, ResourceManager* resourceManager,
        InputManager* inputManager, const SceneContext& context) override;
    void unload() override;
    SceneContext saveState() override;
    void handleEvents(const SDL_Event& event) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::unique_ptr<ISceneLoader> m_sceneLoader;
    std::string m_sceneFilePath;
    std::unique_ptr<RenderSystem> m_renderSystem;
    std::unique_ptr<TilemapRenderSystem> m_tilemapRenderSystem;
    std::unique_ptr<AnimationSystem> m_animationSystem;
    std::unique_ptr<PlayerIntentSystem> m_playerIntentSystem;
    std::unique_ptr<TopDownMovementSystem> m_topDownMovementSystem;
    std::unique_ptr<AnimationStateSystem> m_animationStateSystem;
    std::unique_ptr<CameraSystem> m_cameraSystem;

    ResourceManager* m_resourceManager = nullptr;
    InputManager* m_inputManager = nullptr;

    entt::registry m_registry; // The ECS registry for this scene
};
