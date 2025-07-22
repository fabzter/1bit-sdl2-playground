#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include <optional>

/**
 * @struct CameraComponent
 * @brief Holds data for a camera entity.
 * The CameraSystem will use this to control the viewport.
 */
struct CameraComponent {
    // The entity this camera should follow. Can be null.
    entt::entity target{entt::null};

    // Is this the currently active camera for rendering?
    bool isActive = true;

    // --- Optional Behaviors (to be used later) ---

    // If > 0, the camera smoothly follows. If 0, it's a hard lock.
    float followSpeed = 8.0f;

    // If set, the camera will not move outside these world coordinates.
    std::optional<SDL_FRect> worldBounds;
};
