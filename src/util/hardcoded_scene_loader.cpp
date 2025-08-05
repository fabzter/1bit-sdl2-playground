#include "hardcoded_scene_loader.hpp"
#include "hardcoded_map_loader.hpp"
#include "resource_manager.hpp"
#include "../scenes/definitions/level1.hpp"
#include "../core/context.hpp"

// --- Component Includes ---
#include "../components/tag.hpp"
#include "../components/transform.hpp"
#include "../components/sprite.hpp"
#include "../components/rigidbody.hpp"
#include "../components/collider.hpp"
#include "../components/movement.hpp"
#include "../components/player_control.hpp"
#include "../components/intent.hpp"
#include "../components/camera.hpp"
#include "../components/blackboard.hpp"
#include "../components/behavior.hpp"
#include "../components/statemachine/statemachine.hpp"

// --- Behavior and FSM State Includes ---
#include "../core/behaviors/collectible_behavior.hpp"
#include "../core/fsm/simple_animation_state.hpp"

#include <iostream>

bool HardcodedSceneLoader::load(entt::registry& registry, SDL_Renderer* renderer,
                                ResourceManager* resourceManager, const std::string& sourcePath) {

    // The sourcePath acts as a key to select which hardcoded scene to load.
    if (sourcePath != "Level1") {
        std::cerr << "HardcodedSceneLoader: Unknown source path '" << sourcePath << "'" << std::endl;
        return false;
    }

    // --- Context Setup ---
    int screenW, screenH;
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
    registry.ctx().emplace<ScreenDimensions>(static_cast<float>(screenW), static_cast<float>(screenH));

    // --- Asset Preloading (from Scene Descriptor) ---
    for (const auto& entityDesc : AssetDefinitions::Level1Scene.entities) {
        for (const auto& compDesc : entityDesc.components) {
            if (const auto* spriteDesc = std::get_if<SpriteDescriptor>(&compDesc)) {
                if (!spriteDesc->assetId.empty()) {
                    resourceManager->loadSpriteAsset(renderer, spriteDesc->assetId);
                }
            }
        }
    }

    // --- Entity Creation (Pass 1) ---
    // In the first pass, we create entities and add all components. We also
    // build a map to resolve name-based references in the second pass.
    std::unordered_map<std::string, entt::entity> nameToEntityMap;

    for (const auto& entityDesc : AssetDefinitions::Level1Scene.entities) {
        const auto entity = registry.create();
        nameToEntityMap[entityDesc.name] = entity;

        registry.emplace<TagComponent>(entity, entityDesc.name);

        for (const auto& compDesc : entityDesc.components) {
            std::visit([&](auto&& arg) {
                createComponent(registry, entity, arg);
            }, compDesc);
        }
    }

    // --- Map Loading ---
    // Add the renderer to the context before loading the map.
    registry.ctx().emplace<SDL_Renderer*>(renderer);
    // Now that assets are loaded and entities are being created, delegate map loading.
    loadMapData(registry, resourceManager, AssetDefinitions::Level1Map);
    // Clean up the context after the map is loaded.
    registry.ctx().erase<SDL_Renderer*>();

    // --- Reference Resolution (Pass 2) ---
    // Now we iterate again to populate data that might reference other entities, like the blackboard.
    for (const auto& entityDesc : AssetDefinitions::Level1Scene.entities) {
        const auto entity = nameToEntityMap.at(entityDesc.name);

        if (!entityDesc.blackboard.empty() && registry.all_of<BlackboardComponent>(entity)) {
            auto& blackboard = registry.get<BlackboardComponent>(entity);
            for (const auto& [key, value] : entityDesc.blackboard) {
                // Check if the value is a string that matches an entity name
                if (const auto* str_val = std::any_cast<const char*>(&value)) {
                    if (nameToEntityMap.count(*str_val)) {
                        blackboard.values[key] = nameToEntityMap.at(*str_val);
                        continue;
                    }
                }
                // Otherwise, assign the value as-is
                blackboard.values[key] = value;
            }
        }

        // --- Post-processing: Set Sprite Dimensions from Asset ---
        if (auto* sprite = registry.try_get<SpriteComponent>(entity)) {
            if (const auto* asset = resourceManager->getSpriteAsset(sprite->assetId)) {
                sprite->width = asset->width;
                sprite->height = asset->height;
            }
        }
    }

    // --- Post-processing: Set Active Camera ---
    if (nameToEntityMap.count("Camera")) {
        registry.ctx().emplace<ActiveCamera>(nameToEntityMap.at("Camera"));
    }

    return true;
}


// --- Private Helper Methods ---

void HardcodedSceneLoader::loadMapData(entt::registry& registry, ResourceManager* resourceManager, const TMX::Map& mapDesc) {
    const auto worldEntity = registry.create();
    registry.emplace<TagComponent>(worldEntity, "World");

    // Use the dedicated map loader for this task
    HardcodedMapLoader mapLoader(mapDesc);
    mapLoader.load(registry, worldEntity, *resourceManager, ""); // sourcePath is ignored
}

// --- Component Creation Overloads ---

void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const TransformDescriptor& desc) {
    // Explicitly convert from descriptor's Vec2f to component's Vec2f
    registry.emplace<TransformComponent>(entity,
        Vec2f{desc.position.x, desc.position.y},
        Vec2f{desc.scale.x, desc.scale.y}
    );
}

void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const SpriteDescriptor& desc) {
    auto& sprite = registry.emplace<SpriteComponent>(entity);
    sprite.assetId = desc.assetId;
    sprite.isAnimated = desc.isAnimated;
    sprite.sortingLayer = desc.sortingLayer;
    sprite.orderInLayer = desc.orderInLayer;
}

void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const RigidBodyDescriptor& desc) {
    auto& rb = registry.emplace<RigidBodyComponent>(entity);
    rb.bodyType = static_cast<BodyType>(desc.bodyType);
    rb.mass = desc.mass;
    rb.restitution = desc.restitution;
    rb.damping = desc.damping;
}

void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const ColliderDescriptor& desc) {
    auto& col = registry.emplace<ColliderComponent>(entity);
    col.size = desc.size;
    col.offset = desc.offset;
    col.layer = desc.layer;
    col.mask = desc.mask;
    col.is_static = desc.isStatic;
    col.is_trigger = desc.isTrigger;
}

void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const MovementDescriptor& desc) {
    registry.emplace<MovementComponent>(entity, desc.speed);
}

void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const BehaviorDescriptor& desc) {
    auto& behavior = registry.emplace<BehaviorComponent>(entity);
    if (desc.type == "collectible") {
        behavior.responder = std::make_unique<CollectibleBehavior>();
    }
    // else if (...) for other behaviors
}

void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const StateMachineDescriptor& desc) {
    auto& fsm = registry.emplace<StateMachineComponent>(entity);
    fsm.currentState = entt::hashed_string{desc.initialState.c_str()};

    for (const auto& [name, stateDesc] : desc.states) {
        if (stateDesc.type == "simple") {
            fsm.states[entt::hashed_string{name.c_str()}] =
                std::make_unique<SimpleAnimationState>(entt::hashed_string{stateDesc.animation.c_str()});
        }
    }

    for (const auto& transDesc : desc.transitions) {
        Transition transition;
        transition.toState = entt::hashed_string{transDesc.to.c_str()};
        for (const auto& condDesc : transDesc.conditions) {
            transition.conditions.emplace_back(condDesc.blackboardKey, condDesc.expectedValue);
        }
        fsm.transitions[entt::hashed_string{transDesc.from.c_str()}].push_back(transition);
    }

    // Immediately call onEnter for the initial state
    if (auto it = fsm.states.find(fsm.currentState); it != fsm.states.end()) {
        it->second->onEnter(entity, registry);
    }
}

// --- Tag Component Overloads ---
void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const PlayerControlTag&) {
    registry.emplace<PlayerControlComponent>(entity);
}
void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const IntentTag&) {
    registry.emplace<IntentComponent>(entity);
}
void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const CameraTag&) {
    registry.emplace<CameraComponent>(entity);
}
void HardcodedSceneLoader::createComponent(entt::registry& registry, entt::entity entity, const BlackboardTag&) {
    registry.emplace<BlackboardComponent>(entity);
}