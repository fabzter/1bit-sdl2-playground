#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include <optional>

/**
 * @struct CameraComponent
 * @brief A tag component that identifies an entity as a camera.
 * All camera-specific data (like follow speed, target, etc.) is stored
 * in the entity's BlackboardComponent to allow for data-driven configuration.
 */
struct CameraComponent {};
