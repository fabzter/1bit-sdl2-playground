#pragma once

#include "../core/scene_loader.hpp"
#include <toml++/toml.h>
#include <string>
#include <unordered_map>

/**
 * @class TomlSceneLoader
 * @brief Implements scene loading from a .toml file.
 */
class TomlSceneLoader : public ISceneLoader {
public:
    bool load(entt::registry& registry,
        SDL_Renderer* renderer,
        ResourceManager* resourceManager,
        const std::string& sourcePath) override;

private:
    void parseTransform(entt::registry& registry, entt::entity entity, const toml::table& componentData);
    void parseSprite(entt::registry& registry, entt::entity entity, const toml::table& componentData);
    void parsePlayerControl(entt::registry& registry, entt::entity entity);
    void parseMovement(entt::registry& registry, entt::entity entity, const toml::table& componentData);
    void parseCollider(entt::registry &registry, entt::entity entity, const toml::table &data);
    void parseIntent(entt::registry& registry, entt::entity entity);
    void parseCamera(entt::registry& registry, entt::entity entity);
    void parseTilemap(entt::registry &registry, SDL_Renderer *renderer,
        ResourceManager *resourceManager, const entt::registry::entity_type newEntity,
        toml::impl::table_proxy_pair<false>::value_type &compData);

    // Helpers for the blackboard, which can contain many types
    void parseBlackboard(entt::registry& registry, entt::entity entity, const toml::table& componentData,
                         const std::unordered_map<std::string, entt::entity>& nameToEntityMap);
};
