#include "system_manager.hpp"

void SystemManager::addUpdateSystem(std::unique_ptr<IUpdateSystem> system) {
    m_updateSystems.push_back(std::move(system));
}

void SystemManager::addRenderSystem(std::unique_ptr<IRenderSystem> system) {
    m_renderSystems.push_back(std::move(system));
}

void SystemManager::updateAll(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager, float deltaTime) {
    for (auto& system : m_updateSystems) {
        system->update(registry, inputManager, resourceManager, deltaTime);
    }
}

void SystemManager::drawAll(SDL_Renderer* renderer, entt::registry& registry, ResourceManager& resourceManager) {
    for (auto& system : m_renderSystems) {
        system->draw(renderer, registry, resourceManager);
    }
}
