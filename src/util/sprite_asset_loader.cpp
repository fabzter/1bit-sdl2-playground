#include "sprite_asset_loader.hpp"
#include "text_asset_parser.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

std::unique_ptr<SpriteAsset> SpriteAssetLoader::loadFromFile(SDL_Renderer* renderer, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open sprite file: " << filepath << std::endl;
        return nullptr;
    }

    //get the pixel format
    SDL_PixelFormat* pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    if (!pixelFormat) {
        std::cerr << "Failed to allocate pixel format: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    auto asset = std::make_unique<SpriteAsset>();
    std::vector<std::vector<uint32_t>> atlasFrames;
    TextAssetParser::PaletteMap palette;

    std::string line;
    enum class ParseSection { None, TextureAtlas } currentSection = ParseSection::None;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Handle special comments within a section FIRST
        if (line[0] == '#') {
            if (currentSection == ParseSection::TextureAtlas && line.find("# FRAME_") != std::string::npos) {
                atlasFrames.push_back(TextAssetParser::parsePixelBlock(file, asset->width, asset->height, palette));
            }
            // Otherwise, it's a generic comment, so we skip it.
            continue;
        }

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "SPRITE_NAME") ss >> asset->assetId;
        else if (key == "SPRITE_SIZE") ss >> asset->width >> asset->height;
        else if (key == "PALETTE_BEGIN") palette = TextAssetParser::parsePalette(file, pixelFormat);
        else if (key == "TEXTURE_ATLAS_BEGIN") currentSection = ParseSection::TextureAtlas;
        else if (key == "TEXTURE_ATLAS_END") currentSection = ParseSection::None;
        else if (key == "ANIMATION_BEGIN") {
            // Animation parsing logic remains the same...
            std::string animName;
            ss >> animName;
            AnimationSequence sequence;
            int currentDuration = 100;

            while(std::getline(file, line) && line.find("ANIMATION_END") == std::string::npos) {
                if (line.empty() || line[0] == '#') continue;
                std::stringstream anim_ss(line);
                std::string animKey;
                anim_ss >> animKey;
                if (animKey == "DURATION") anim_ss >> currentDuration;
                else if (animKey == "FRAME") {
                    int frameIndex;
                    anim_ss >> frameIndex;
                    sequence.push_back({frameIndex, currentDuration});
                }
            }
            asset->animations[entt::hashed_string{animName.c_str()}] = sequence;
        }
    }

    SDL_FreeFormat(pixelFormat);

    if (asset->assetId.empty() || asset->width == 0 || asset->height == 0 || atlasFrames.empty()) {
        std::cerr << "Invalid or empty sprite file: " << filepath << std::endl;
        return nullptr;
    }

    // --- Texture Atlas creation logic remains the same ---
    const int atlasWidth = asset->width * atlasFrames.size();
    const int atlasHeight = asset->height;
    SDL_Texture* rawTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, atlasWidth, atlasHeight);
    if (!rawTexture) {
        std::cerr << "Failed to create texture atlas: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_SetTextureBlendMode(rawTexture, SDL_BLENDMODE_BLEND);

    for (size_t i = 0; i < atlasFrames.size(); ++i) {
        if(atlasFrames[i].size() != (size_t)asset->width * asset->height) continue;
        SDL_Rect destRect = { static_cast<int>(i) * asset->width, 0, asset->width, asset->height };
        SDL_UpdateTexture(rawTexture, &destRect, atlasFrames[i].data(), asset->width * sizeof(uint32_t));
    }

    asset->textureAtlas.reset(rawTexture);
    return asset;
}
