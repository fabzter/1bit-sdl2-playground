#include "camera_system.hpp"
#include "../components/camera.hpp"
#include "../components/transform.hpp"
#include <cmath>

// Linear interpolation function
static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void CameraSystem::update(entt::registry& registry, float deltaTime) {
    entt::entity activeCameraEntity = entt::null;
    const CameraComponent* activeCamComponent = nullptr;

    //find the active camera and its target
    auto cameraView = registry.view<const CameraComponent, TransformComponent>();
    for (auto entity : cameraView) {
        const auto& cam = cameraView.get<const CameraComponent>(entity);
        if (cam.isActive) {
            activeCameraEntity = entity;
            activeCamComponent = &cam;
            break; // no need to search further
        }
    }

    // if no active camera, or no target, there's nothing to do
    if (activeCameraEntity == entt::null ||
        !activeCamComponent || activeCamComponent->target == entt::null) {
        return;
    }

    // get the transform components for camera and target
    auto& cameraTransform = registry.get<TransformComponent>(activeCameraEntity);
    const auto& targetTransform = registry.get<const TransformComponent>(activeCamComponent->target);

    // --- Dead Zone Check ---
    const float dx = targetTransform.position.x - cameraTransform.position.x;
    const float dy = targetTransform.position.y - cameraTransform.position.y;
    const float distance = std::sqrt(dx * dx + dy * dy);

    // if the target is within the dead zone, do not move the camera
    if (distance <= activeCamComponent->deadZoneRadius) {
        return;
    }

    if (activeCamComponent->followSpeed > 0.0f) {
        // --- Smooth Follow (Frame-rate independent Lerp) ---
        // This formula ensures the smoothing feels consistent regardless of the frame rate.
        const float smoothingFactor = 1.0f - std::exp(-activeCamComponent->followSpeed * deltaTime);
        cameraTransform.position.x = lerp(cameraTransform.position.x, targetTransform.position.x, smoothingFactor);
        cameraTransform.position.y = lerp(cameraTransform.position.y, targetTransform.position.y, smoothingFactor);
    } else {
        // --- Hard Lock Logic (Fallback) ---
        cameraTransform.position = targetTransform.position;
    }
}
