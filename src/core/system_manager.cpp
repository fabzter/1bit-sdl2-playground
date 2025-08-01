#include "system_manager.hpp"

void SystemManager::addUpdateSystem(std::unique_ptr<IUpdateSystem> system) {
    m_updateSystems.push_back(std::move(system));
}

void SystemManager::addRenderSystem(std::unique_ptr<IRenderSystem> system) {
    m_renderSystems.push_back(std::move(system));
}

void SystemManager::initAll(entt::registry& registry) {
    for (auto& system : m_updateSystems) {
        system->init(registry);
    }
    for (auto& system : m_renderSystems) {
        system->init(registry);
    }
}

void SystemManager::updateAll(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager, float deltaTime) {
    for (auto& system : m_updateSystems) {
        system->update(registry, inputManager, resourceManager, deltaTime);
    }

    // Process all enqueued events and notify listeners.
    registry.ctx().get<entt::dispatcher>().update();
}

void SystemManager::drawAll(SDL_Renderer* renderer, entt::registry& registry, ResourceManager& resourceManager) {
    for (auto& system : m_renderSystems) {
        system->draw(renderer, registry, resourceManager);
    }
}
