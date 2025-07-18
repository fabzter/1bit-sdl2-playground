#pragma once

#include "../core/scene.hpp"
#include "../systems/renderer.hpp"
#include "../systems/animation.hpp"
#include <memory>

class ResourceManager; // Forward declaration

class GameScene : public Scene {
public:
    GameScene() = default;
    ~GameScene() override = default;

    void load(SDL_Renderer* renderer, ResourceManager* resourceManager) override;
    void unload() override;
    void handleEvents(const SDL_Event& event) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::unique_ptr<RenderSystem> m_renderSystem;
    std::unique_ptr<AnimationSystem> m_animationSystem;
    ResourceManager* m_resourceManager = nullptr;
    entt::registry m_registry; // The ECS registry for this scene
};
