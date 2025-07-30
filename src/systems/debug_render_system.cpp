#include "debug_render_system.hpp"
#include "../components/transform.hpp"
#include "../components/collider.hpp"
#include "../core/context.hpp"
#include "../util/quadtree.hpp" // For QuadtreeRect

// We can re-use our helper function from the CollisionSystem
static QuadtreeRect getDebugEntityBounds(const TransformComponent& transform, const ColliderComponent& collider) {
    const float scaledWidth = collider.size.x * transform.scale.x;
    const float scaledHeight = collider.size.y * transform.scale.y;
    const float scaledOffsetX = collider.offset.x * transform.scale.x;
    const float scaledOffsetY = collider.offset.y * transform.scale.y;

    return {
        static_cast<int>(transform.position.x - (scaledWidth / 2.0f) + scaledOffsetX),
        static_cast<int>(transform.position.y - (scaledHeight / 2.0f) + scaledOffsetY),
        static_cast<int>(scaledWidth),
        static_cast<int>(scaledHeight)
    };
}

void DebugRenderSystem::draw(SDL_Renderer* renderer, entt::registry& registry, ResourceManager&) {
    // For now, we'll always draw. Later we can toggle this with a key press.
    const auto& view = registry.view<const TransformComponent, const ColliderComponent>();
    if (view.size_hint() == 0 ||!registry.ctx().contains<ActiveCamera>()) {
        return;
    }

    const auto cameraEntity = registry.ctx().get<ActiveCamera>().entity;
    const auto& screen = registry.ctx().get<ScreenDimensions>();
    const auto& camTransform = registry.get<const TransformComponent>(cameraEntity);
    const float cameraOffsetX = camTransform.position.x - screen.w / 2.0f;
    const float cameraOffsetY = camTransform.position.y - screen.h / 2.0f;

    for (const auto entity : view) {
        const auto& [transform, collider] = view.get<const TransformComponent, const ColliderComponent>(entity);

        QuadtreeRect bounds = getDebugEntityBounds(transform, collider);
        SDL_Rect debugRect = {
            bounds.x - static_cast<int>(cameraOffsetX),
            bounds.y - static_cast<int>(cameraOffsetY),
            bounds.w,
            bounds.h
        };

        if (collider.is_trigger) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for triggers
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);   // Green for solid colliders
        }

        SDL_RenderDrawRect(renderer, &debugRect);
    }
}
