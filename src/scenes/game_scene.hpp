#pragma once

#include "../core/scene.hpp"
#include "../systems/renderer.hpp"
#include <memory>
#include <entt/entt.hpp>

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
    ResourceManager* m_resourceManager = nullptr; // Non-owning pointer
    entt::registry m_registry; // The ECS registry for this scene
};
