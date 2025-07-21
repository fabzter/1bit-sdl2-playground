#pragma once

#include <memory>
#include "../core/scene.hpp"
#include "../systems/renderer.hpp"
#include "../systems/animation.hpp"
#include "../systems/player_intent_system.hpp"
#include "../systems/top_down_movement_system.hpp"

// Forward-declare systems and managers
class RenderSystem;
class AnimationSystem;
class PlayerIntentSystem;
class TopDownMovementSystem;

class ResourceManager;
class InputManager;

class GameScene : public Scene {
public:
    GameScene(
        std::unique_ptr<PlayerIntentSystem> playerIntentSystem,
        std::unique_ptr<TopDownMovementSystem> topDownMovementSystem,
        std::unique_ptr<AnimationSystem> animationSystem,
        std::unique_ptr<RenderSystem> renderSystem
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
    std::unique_ptr<RenderSystem> m_renderSystem;
    std::unique_ptr<AnimationSystem> m_animationSystem;
    std::unique_ptr<PlayerIntentSystem> m_playerIntentSystem;
    std::unique_ptr<TopDownMovementSystem> m_topDownMovementSystem;

    ResourceManager* m_resourceManager = nullptr;
    InputManager* m_inputManager = nullptr;

    entt::registry m_registry; // The ECS registry for this scene
};
