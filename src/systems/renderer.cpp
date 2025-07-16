#include "renderer.hpp"
#include <iostream>

RenderSystem::~RenderSystem() {
}

void RenderSystem::draw(SDL_Renderer* renderer, ResourceManager& resourceManager) {
    // Lazy load the texture on the first draw call
    SDL_Texture* playerTexture = resourceManager.getTexture(renderer, "player");

    if (playerTexture) {
        int texW = 0, texH = 0;
        SDL_QueryTexture(playerTexture, NULL, NULL, &texW, &texH);
        
        int screenW = 0, screenH = 0;
        SDL_GetRendererOutputSize(renderer, &screenW, &screenH);

        float scale = 5.0f;
        SDL_FRect destRect = {
            (screenW - texW * scale) / 2.0f,
            (screenH - texH * scale) / 2.0f,
            texW * scale,
            texH * scale
        };
        SDL_RenderCopyF(renderer, playerTexture, NULL, &destRect);
    } else {
        // Add an error message in case the texture is null
        std::cerr << "RenderSystem::draw - playerTexture is null!" << std::endl;
    }
}
