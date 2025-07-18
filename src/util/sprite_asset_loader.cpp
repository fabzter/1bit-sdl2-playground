#include "sprite_asset_loader.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

std::unique_ptr<SpriteAsset> SpriteAssetLoader::loadFromFile(SDL_Renderer* renderer, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open sprite file: " << filepath << std::endl;
        return nullptr;
    }

    auto asset = std::make_unique<SpriteAsset>();
    std::unordered_map<char, uint32_t> palette;
    std::vector<std::vector<uint32_t>> atlasFrames;
    std::string line, key;

    // Set up a default palette in case the file doesn't specify one.
    palette['0'] = 0x00000000; // Transparent
    palette['1'] = 0xFFE0E0E0; // Opaque off-white

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        ss >> key;

        if (key == "SPRITE_NAME") {
            ss >> asset->assetId;
        } else if (key == "SPRITE_SIZE") {
            ss >> asset->width >> asset->height;
        } else if (key == "PALETTE_BEGIN") {
            palette.clear(); // A palette is being defined, so clear the default.
            while(std::getline(file, line) && line.find("PALETTE_END") == std::string::npos) {
                if (line.empty() || line[0] == '#') continue;

                std::stringstream pss(line);
                char index_char;
                int r, g, b, a;
                if (pss >> index_char >> r >> g >> b >> a) {
                    uint32_t color = (static_cast<uint32_t>(a) << 24) |
                                     (static_cast<uint32_t>(b) << 16) |
                                     (static_cast<uint32_t>(g) << 8)  |
                                      static_cast<uint32_t>(r);
                    palette[index_char] = color;
                }
            }
        } else if (key == "TEXTURE_ATLAS_BEGIN") {
            while (std::getline(file, line) && line.find("TEXTURE_ATLAS_END") == std::string::npos) {
                if (line.empty()) continue;
                // A line starting with '#' inside the atlas block denotes the start of a new frame's pixel data.
                if (line[0] == '#') {
                    atlasFrames.emplace_back();
                    atlasFrames.back().reserve(asset->width * asset->height);
                    for (int y = 0; y < asset->height; ++y) {
                        // Find the next non-comment, non-empty line for pixel data
                        do {
                           if (!std::getline(file, line)) { line = ""; break; }
                        } while(line.empty() || line[0] == '#');
                        if (line.empty()) break; // Reached end of file

                        for (int x = 0; x < asset->width; ++x) {
                           char pixel_char = (x < line.length()) ? line[x] : '0';
                           auto it = palette.find(pixel_char);
                           if (it != palette.end()) {
                               atlasFrames.back().push_back(it->second);
                           } else {
                               atlasFrames.back().push_back(0xFFFF00FF); // Magenta for error
                           }
                        }
                    }
                }
            }
        } else if (key == "ANIMATION_BEGIN") {
            std::string animName;
            ss >> animName;
            AnimationSequence sequence;
            int currentDuration = 100; // Default duration
            while(std::getline(file, line) && line.find("ANIMATION_END") == std::string::npos) {
                if (line.empty() || line[0] == '#') continue;
                std::stringstream anim_ss(line);
                std::string animKey;
                anim_ss >> animKey;
                if (animKey == "DURATION") {
                    anim_ss >> currentDuration;
                } else if (animKey == "FRAME") {
                    int frameIndex;
                    anim_ss >> frameIndex;
                    sequence.push_back({frameIndex, currentDuration});
                }
            }
            asset->animations[animName] = sequence;
        }
    }

    if (asset->assetId.empty() || asset->width == 0 || asset->height == 0 || atlasFrames.empty()) {
        std::cerr << "Invalid sprite file format or missing critical data (SPRITE_NAME, SPRITE_SIZE, or TEXTURE_ATLAS) in: " << filepath << std::endl;
        return nullptr;
    }

    // Create the single texture atlas to hold all frames
    const int atlasWidth = asset->width * atlasFrames.size();
    const int atlasHeight = asset->height;
    SDL_Texture* rawTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, atlasWidth, atlasHeight);

    if (!rawTexture) {
        std::cerr << "Failed to create texture atlas: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_SetTextureBlendMode(rawTexture, SDL_BLENDMODE_BLEND);

    // Copy each frame's pixel data into the correct position in the atlas
    for (size_t i = 0; i < atlasFrames.size(); ++i) {
        SDL_Rect destRect = { static_cast<int>(i) * asset->width, 0, asset->width, asset->height };
        SDL_UpdateTexture(rawTexture, &destRect, atlasFrames[i].data(), asset->width * sizeof(uint32_t));
    }

    asset->textureAtlas.reset(rawTexture);
    return asset;
}
