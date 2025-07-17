#include "sprite_asset_loader.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

std::unique_ptr<SpriteAsset> SpriteAssetLoader::loadFromFile(SDL_Renderer* renderer,
    const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open sprite file: " << filepath << std::endl;
        return nullptr;
    }

    auto asset = std::make_unique<SpriteAsset>();

    std::vector<uint32_t> pixels;
    std::unordered_map<char, uint32_t> palette;
    std::string line;

    // Set up a default palette in case the file doesn't specify one.
    // This maintains backward compatibility with the old format.
    // For SDL_PIXELFORMAT_RGBA8888, the uint32_t color is (A<<24)|(B<<16)|(G<<8)|R.
    palette['0'] = 0x00000000; // Transparent background
    palette['1'] = 0xFFE0E0E0; // Opaque off-white foreground

    // parse
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "SPRITE_NAME") {
            ss >> asset->assetId; // <-- Assign assetId from file content
        }
        else if (key == "SPRITE_SIZE") {
            ss >> asset->width >> asset->height;
        } else if (key == "PALETTE_BEGIN") {
            palette.clear(); // A palette is being defined, so clear the default.
            while(std::getline(file, line) && line.find("PALETTE_END") == std::string::npos) {
                if (line.empty() || line[0] == '#') continue;

                std::stringstream pss(line);
                char index_char;
                int r, g, b, a;
                if (pss >> index_char >> r >> g >> b >> a) {
                    // Pack RGBA into a 32-bit integer for the texture
                    uint32_t color = (static_cast<uint32_t>(a) << 24) |
                                     (static_cast<uint32_t>(b) << 16) |
                                     (static_cast<uint32_t>(g) << 8)  |
                                      static_cast<uint32_t>(r);
                    palette[index_char] = color;
                }
            }
        } else if (key == "STATE") {
            // For now, we only load the first state we find
            // In the future, this would handle animations (idle, walk, etc.)
            pixels.reserve(asset->width * asset->height);
            for (int y = 0; y < asset->height; ++y) {
                // Find the next non-comment, non-empty line for pixel data
                do {
                    if (!std::getline(file, line)) {
                        line = ""; // Signal end of file
                        break;
                    }
                } while (line.empty() || line[0] == '#');

                if (line.empty()) break; // Reached end of file before reading all rows

                for (int x = 0; x < asset->width; ++x) {
                    char pixelChar = (x < line.length()) ? line[x] : '0';
                    auto it = palette.find(pixelChar);
                    if (it != palette.end()) {
                        pixels.push_back(it->second);
                    } else { // Fallback for a character not in the palette
                        pixels.push_back(0xFFFF00FF); // Use a bright magenta for errors
                    }
                }
            }
            break; // Stop after reading one state's data
        }
    }

    //TODO: accept assets without id, generate a random guid.
    if (asset->assetId.empty() || asset->width == 0 || asset->height == 0 || pixels.empty()) {
        std::cerr << "Invalid or incomplete sprite file: " << filepath << std::endl;
        return nullptr;
    }

    SDL_Texture* rawTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC, asset->width, asset->height);
    if (!rawTexture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_SetTextureBlendMode(rawTexture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(rawTexture, NULL, pixels.data(), asset->width * sizeof(uint32_t));

    asset->texture.reset(rawTexture);
    return asset;
}