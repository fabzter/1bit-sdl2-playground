#pragma once

#include <memory>
#include "../core/scene.hpp"
#include "../core/scene_loader.hpp"
#include "../core/system_manager.hpp"

// Forward-declare managers
class ResourceManager;
class InputManager;

class GameScene : public Scene {
public:
    GameScene(std::unique_ptr<ISceneLoader> sceneLoader, std::string sceneFilePath);
    ~GameScene() override = default;

    void setSystemManager(std::unique_ptr<SystemManager> systemManager);
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
    std::unique_ptr<SystemManager> m_systemManager;

    ResourceManager* m_resourceManager = nullptr;
    InputManager* m_inputManager = nullptr;

    entt::registry m_registry; // The ECS registry for this scene
};
