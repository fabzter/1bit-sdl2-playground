#pragma once

#include <vector>
#include <memory>
#include "systems/isystem.hpp"

class SystemManager {
public:
    void addUpdateSystem(std::unique_ptr<IUpdateSystem> system);
    void addRenderSystem(std::unique_ptr<IRenderSystem> system);

    void initAll(entt::registry& registry);
    void updateAll(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager, float deltaTime);
    void drawAll(SDL_Renderer* renderer, entt::registry& registry, ResourceManager& resourceManager);

private:
    std::vector<std::unique_ptr<IUpdateSystem>> m_updateSystems;
    std::vector<std::unique_ptr<IRenderSystem>> m_renderSystems;
};
