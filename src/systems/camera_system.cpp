#include "camera_system.hpp"
#include "../components/camera.hpp"
#include "../components/transform.hpp"
#include "../components/blackboard.hpp"
#include "../core/context.hpp"
#include  "../core/blackboard_keys.hpp"
#include <algorithm>
#include <cmath>

// Linear interpolation function
static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void CameraSystem::update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) {
    if (!registry.ctx().contains<ActiveCamera>()) return; // No camera to update
    const auto cameraEntity = registry.ctx().get<ActiveCamera>().entity;
    if (!registry.valid(cameraEntity) || !registry.all_of<BlackboardComponent>(cameraEntity)) return;

    // --- Read config from the blackboard ---
    const auto& blackboard = registry.get<const BlackboardComponent>(cameraEntity);
    entt::entity target = entt::null;
    float followSpeed = 0.0f; // 0.0f means hard lock
    float deadZoneRadius = 0.0f;

    if (auto it = blackboard.values.find(BlackboardKeys::Camera::Target); it != blackboard.values.end()) {
        target = std::any_cast<entt::entity>(it->second);
    }
    if (auto it = blackboard.values.find(BlackboardKeys::Camera::FollowSpeed); it != blackboard.values.end()) {
        followSpeed = std::any_cast<float>(it->second);
    }
    if (auto it = blackboard.values.find(BlackboardKeys::Camera::DeadZoneRadius); it != blackboard.values.end()) {
        deadZoneRadius = std::any_cast<float>(it->second);
    }

    if (target == entt::null || !registry.valid(target)) return;

    // get the transform components for camera and target
    auto& cameraTransform = registry.get<TransformComponent>(cameraEntity);
    const auto& targetTransform = registry.get<const TransformComponent>(target);

    // --- Dead Zone Check ---
    const float dx = targetTransform.position.x - cameraTransform.position.x;
    const float dy = targetTransform.position.y - cameraTransform.position.y;
    const float distance = std::sqrt(dx * dx + dy * dy);
    // if the target is within the dead zone, do not move the camera
    if (distance <= deadZoneRadius) {
        return;
    }

    // -- Behavior Selection ---
    if (followSpeed > 0.0f) {
        // --- Smooth Follow (Frame-rate independent Lerp) ---
        // This formula ensures the smoothing feels consistent regardless of the frame rate.
        const float smoothingFactor = 1.0f - std::exp(-followSpeed * deltaTime);
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
