#include "toml_scene_loader.hpp"
#include "resource_manager.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/player_control.hpp"
#include "../components/movement.hpp"
#include "../components/camera.hpp"
#include "../components/blackboard.hpp"
#include "../core/context.hpp"
#include "../core/blackboard_keys.hpp"
#include <iostream>

#include "../components/intent.hpp"

// Main loading function
bool TomlSceneLoader::load(entt::registry& registry,
    SDL_Renderer* renderer,
    ResourceManager* resourceManager,
    const std::string& sourcePath) {
    try {
        toml::table sceneData = toml::parse_file(sourcePath);

        // --- Load world and context data first ---
        // Set ScreenDimensions from the renderer
        int screenW, screenH;
        SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
        registry.ctx().emplace<ScreenDimensions>(static_cast<float>(screenW), static_cast<float>(screenH));

        // Load WorldBounds from the TOML file
        if (auto worldData = sceneData["world"].as_table()) {
            if (auto bounds = worldData->get("bounds")->as_array()) {
                SDL_FRect worldBounds = {
                    bounds->get(0)->value_or(0.0f),
                    bounds->get(1)->value_or(0.0f),
                    bounds->get(2)->value_or(1280.0f),
                    bounds->get(3)->value_or(720.0f)
                };
                registry.ctx().emplace<WorldBounds>(worldBounds);
            }
        }

        // --- Preload all required assets first ---
        std::vector<std::string> assetsToPreload;
        if (auto entitiesArray = sceneData["entities"].as_array()) {
            for (auto& elem : *entitiesArray) {
                if (auto entityData = elem.as_table()) {
                    if (auto components = entityData->get("components")->as_table()) {
                        if (auto spriteData = components->get("Sprite")) {
                            assetsToPreload.push_back(spriteData->as_table()->get("assetId")->value_or<std::string>(""));
                        }
                    }
                }
            }
        }
        // Use the explicit loader to load textures into the resource manager
        for(const auto& assetId : assetsToPreload) {
            if(!assetId.empty()) {
                resourceManager->loadSpriteAsset(renderer, assetId);
            }
        }

        // A map to resolve name-based entity references in the second pass
        std::unordered_map<std::string, entt::entity> nameToEntityMap;

        // --- PASS 1: Create entities and parse non-referential components ---
        if (auto entitiesArray = sceneData["entities"].as_array()) {
            for (auto& elem : *entitiesArray) {
                if (auto entityData = elem.as_table()) {
                    const auto entityName = entityData->get("name")->value_or<std::string>("");
                    if (entityName.empty()) continue;

                    const auto newEntity = registry.create();
                    nameToEntityMap[entityName] = newEntity;

                    if (auto components = entityData->get("components")->as_table()) {
                        for (auto& [compName, compData] : *components) {
                            if (compName == "Transform") parseTransform(registry, newEntity, *compData.as_table());
                            else if (compName == "Sprite") parseSprite(registry, newEntity, *compData.as_table());
                            else if (compName == "PlayerControl") parsePlayerControl(registry, newEntity);
                            else if (compName == "Intent") parseIntent(registry, newEntity);
                            else if (compName == "Movement") parseMovement(registry, newEntity, *compData.as_table());
                            else if (compName == "Camera") parseCamera(registry, newEntity);
                            // NOTE: We skip the Blackboard in Pass 1 because it might contain entity references.
                        }
                    }
                }
            }
        }

        // --- PASS 2: Parse components that may contain entity references (like the Blackboard) ---
        if (auto entitiesArray = sceneData["entities"].as_array()) {
            for (auto& elem : *entitiesArray) {
                if (auto entityData = elem.as_table()) {
                    const auto entityName = entityData->get("name")->value_or<std::string>("");
                    if (entityName.empty()) continue;

                    const auto entity = nameToEntityMap.at(entityName);

                    // If the entity has a sprite, update its dimensions from the loaded asset
                    if (registry.all_of<SpriteComponent>(entity)) {
                        auto& sprite = registry.get<SpriteComponent>(entity);
                        if (const auto* asset = resourceManager->getSpriteAsset(sprite.assetId)) {
                            sprite.width = asset->width;
                            sprite.height = asset->height;
                        }
                    }

                    if (auto components = entityData->get("components")->as_table()) {
                         if (auto blackboardData = components->get("Blackboard")) {
                             parseBlackboard(registry, entity, *blackboardData->as_table(), nameToEntityMap);
                         }
                    }

                    // --- Robust Camera Setup ---
                    if (registry.all_of<CameraComponent>(entity)) {
                        // Set this camera as the active one in the context
                        registry.ctx().emplace<ActiveCamera>(entity);

                        // If the camera has a target in its blackboard, sync its position now
                        const auto& blackboard = registry.get<BlackboardComponent>(entity);
                        if (auto it = blackboard.values.find(BlackboardKeys::Camera::Target); it != blackboard.values.end()) {
                            entt::entity targetEntity = std::any_cast<entt::entity>(it->second);
                            if (registry.valid(targetEntity) && registry.all_of<TransformComponent>(targetEntity)) {
                                const auto& targetTransform = registry.get<TransformComponent>(targetEntity);
                                auto& cameraTransform = registry.get<TransformComponent>(entity);
                                cameraTransform.position = targetTransform.position;
                                std::cout << "TomlSceneLoader: Synced camera's initial position to target '" << entityName << "'." << std::endl;
                            }
                        }
                    }
                    // --- END SECTION ---
                }
            }
        }

    } catch (const toml::parse_error& err) {
        std::cerr << "TOML Parsing failed:\n" << err << "\n";
        return false;
    }
    return true;
}

// --- Private Helper Implementations ---

void TomlSceneLoader::parseTransform(entt::registry& registry, entt::entity entity, const toml::table& data) {
    Vec2f pos = {data["position"][0].value_or(0.0f), data["position"][1].value_or(0.0f)};
    Vec2f scale = {data["scale"][0].value_or(1.0f), data["scale"][1].value_or(1.0f)};
    registry.emplace<TransformComponent>(entity, pos, scale);
}

void TomlSceneLoader::parseSprite(entt::registry& registry, entt::entity entity, const toml::table& data) {
    auto assetId = data["assetId"].value_or<std::string>("");
    auto isAnimated = data["isAnimated"].value_or(false);

    auto sortingLayer = data["sortingLayer"].value_or<int16_t>(0);
    auto orderInLayer = data["orderInLayer"].value_or<int16_t>(0);

    // Note: Sprite width/height will be set from the asset later.
    auto& sprite = registry.emplace<SpriteComponent>(entity, assetId, 0, 0, sortingLayer, orderInLayer);
    sprite.isAnimated = isAnimated;
}

void TomlSceneLoader::parsePlayerControl(entt::registry& registry, entt::entity entity) {
    registry.emplace<PlayerControlComponent>(entity);
}

void TomlSceneLoader::parseIntent(entt::registry& registry, entt::entity entity) {
    registry.emplace<IntentComponent>(entity);
}

void TomlSceneLoader::parseMovement(entt::registry& registry, entt::entity entity, const toml::table& data) {
    auto speed = data["speed"].value_or(0.0f);
    registry.emplace<MovementComponent>(entity, speed);
}

void TomlSceneLoader::parseCamera(entt::registry& registry, entt::entity entity) {
    registry.emplace<CameraComponent>(entity);
}

void TomlSceneLoader::parseBlackboard(entt::registry& registry, entt::entity entity, const toml::table& data,
                                      const std::unordered_map<std::string, entt::entity>& nameToEntityMap) {
    auto& blackboard = registry.get_or_emplace<BlackboardComponent>(entity);

    for (const auto& [key, val] : data) {
        std::string keyStr = std::string(key.str());

        // Check the TOML node's type and insert the corresponding C++ type into the std::any map.
        if (val.is_string()) {
            // Special case: If this string is an entity name reference, store the entity handle.
            // Otherwise, store it as a plain string.
            std::string str_val = val.as_string()->get();
            if (nameToEntityMap.count(str_val)) {
                blackboard.values[keyStr] = nameToEntityMap.at(str_val);
            } else {
                blackboard.values[keyStr] = str_val;
            }
        } else if (val.is_boolean()) {
            blackboard.values[keyStr] = val.as_boolean()->get();
        } else if (val.is_floating_point()) {
            blackboard.values[keyStr] = static_cast<float>(val.as_floating_point()->get());
        } else if (val.is_integer()) {
            // TOML integers are 64-bit, so cast to a reasonable default like int
            blackboard.values[keyStr] = static_cast<int>(val.as_integer()->get());
        }
    }
}
