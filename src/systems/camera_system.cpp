#include "camera_system.hpp"
#include "../components/camera.hpp"
#include "../components/transform.hpp"
#include "../core/context.hpp"
#include <algorithm>
#include <cmath>

// Linear interpolation function
static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void CameraSystem::update(entt::registry& registry, float deltaTime) {
    if (!registry.ctx().contains<ActiveCamera>()) return; // No camera to update
    const auto cameraEntity = registry.ctx().get<ActiveCamera>().entity;
    if (!registry.valid(cameraEntity)) return;

    const auto& cam = registry.get<CameraComponent>(cameraEntity);

    // if no active camera, or no target, there's nothing to do
    if (cam.target == entt::null || !registry.valid(cam.target)) {
        return;
    }

    // get the transform components for camera and target
    auto& cameraTransform = registry.get<TransformComponent>(cameraEntity);
    const auto& targetTransform = registry.get<const TransformComponent>(cam.target);

    // --- Dead Zone Check ---
    const float dx = targetTransform.position.x - cameraTransform.position.x;
    const float dy = targetTransform.position.y - cameraTransform.position.y;
    const float distance = std::sqrt(dx * dx + dy * dy);

    // if the target is within the dead zone, do not move the camera
    if (distance <= cam.deadZoneRadius) {
        return;
    }

    // -- Behavior Selection ---
    if (cam.followSpeed > 0.0f) {
        // --- Smooth Follow (Frame-rate independent Lerp) ---
        // This formula ensures the smoothing feels consistent regardless of the frame rate.
        const float smoothingFactor = 1.0f - std::exp(-cam.followSpeed * deltaTime);
        cameraTransform.position.x = lerp(cameraTransform.position.x, targetTransform.position.x, smoothingFactor);
        cameraTransform.position.y = lerp(cameraTransform.position.y, targetTransform.position.y, smoothingFactor);
    } else {
        // --- Hard Lock Logic (Fallback) ---
        cameraTransform.position = targetTransform.position;
    }

    // --- Bounding Logic ---
    // After all movement, clamp the camera's position to the world bounds, if they exist.
    if (registry.ctx().contains<WorldBounds>()) {
        const auto& screen = registry.ctx().get<ScreenDimensions>();
        const auto& bounds = registry.ctx().get<WorldBounds>().rect;
        const float halfViewWidth = screen.w / 2.0f;
        const float halfViewHeight = screen.h / 2.0f;

        // Clamp the camera's center position
        cameraTransform.position.x = std::clamp(cameraTransform.position.x, bounds.x + halfViewWidth, bounds.x + bounds.w - halfViewWidth);
        cameraTransform.position.y = std::clamp(cameraTransform.position.y, bounds.y + halfViewHeight, bounds.y + bounds.h - halfViewHeight);
    }
}
