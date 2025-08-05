#pragma once
#include "../core/scene_loader.hpp"
#include "../core/descriptors/scene/scene_descriptor.hpp"
#include "../core/descriptors/map/TMX.hpp"

//implements the scene loading interface
// but uses hardcoded C++ descriptors as its data source.
class HardcodedSceneLoader : public ISceneLoader {
public:
    HardcodedSceneLoader() = default;

    bool load(entt::registry& registry,
              SDL_Renderer* renderer,
              ResourceManager* resourceManager,
              const std::string& sourcePath) override;

private:
    void loadMapData(entt::registry& registry, ResourceManager* resourceManager, const TMX::Map& mapDesc);
    void createComponent(entt::registry& registry, entt::entity entity, const TransformDescriptor& desc);
    void createComponent(entt::registry &registry, entt::entity entity, const SpriteDescriptor &desc);
    void createComponent(entt::registry &registry, entt::entity entity, const RigidBodyDescriptor &desc);
    void createComponent(entt::registry &registry, entt::entity entity, const ColliderDescriptor &desc);
    void createComponent(entt::registry &registry, entt::entity entity, const MovementDescriptor &desc);
    void createComponent(entt::registry &registry, entt::entity entity, const BehaviorDescriptor &desc);
    void createComponent(entt::registry &registry, entt::entity entity, const StateMachineDescriptor &desc);
    void createComponent(entt::registry &registry, entt::entity entity, const PlayerControlTag&);
    void createComponent(entt::registry &registry, entt::entity entity, const IntentTag&);
    void createComponent(entt::registry &registry, entt::entity entity, const CameraTag&);
    void createComponent(entt::registry &registry, entt::entity entity, const BlackboardTag&);

    // We will add a method here later for loading scene entities (Player, etc.)
};
