#include "camera_system.hpp"
#include "../components/camera.hpp"
#include "../components/transform.hpp"
#include <iostream>

// Linear interpolation function
static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void CameraSystem::update(entt::registry& registry, float deltaTime) {
    entt::entity activeCameraEntity = entt::null;
    entt::entity targetEntity = entt::null;

    //find the active camera and its target
    auto cameraView = registry.view<const CameraComponent>();
    for (auto entity : cameraView) {
        const auto& camera = cameraView.get<const CameraComponent>(entity);
        if (camera.isActive) {
            activeCameraEntity = entity;
            targetEntity = camera.target;
            break; // no need to search further
        }
    }

    // if no active camera, or no target, there's nothing to do
    if (activeCameraEntity == entt::null || targetEntity == entt::null) {
        return;
    }

    // get the transform components for camera and target
    auto& cameraTransform = registry.get<TransformComponent>(activeCameraEntity);
    const auto& targetTransform = registry.get<const TransformComponent>(targetEntity);

    // --- Hard Lock Logic ---
    // The camera's position is set directly to the target's position.
    cameraTransform.position = targetTransform.position;
}
