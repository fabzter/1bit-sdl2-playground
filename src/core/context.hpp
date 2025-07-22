#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>

struct ScreenDimensions {
    float w = 0.0f;
    float h = 0.0f;
};

struct WorldBounds {
    SDL_FRect rect;
};

struct ActiveCamera {
    entt::entity entity{entt::null};
};
