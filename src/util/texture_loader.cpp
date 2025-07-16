#include "texture_loader.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

SDL_Texture* TextureLoader::loadFromSpriteFile(SDL_Renderer* renderer, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open sprite file: " << filepath << std::endl;
        return nullptr;
    }

    int width = 0, height = 0;
    std::vector<uint32_t> pixels;
    std::unordered_map<char, uint32_t> palette;
    std::string line;

    // Set up a default palette in case the file doesn't specify one.
    // This maintains backward compatibility with the old format.
    // For SDL_PIXELFORMAT_RGBA8888, the uint32_t color is (A<<24)|(B<<16)|(G<<8)|R.
    palette['0'] = 0x00000000; // Transparent background
    palette['1'] = 0xFFE0E0E0; // Opaque off-white foreground

    // Enhanced parsing logic
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "SPRITE_SIZE") {
            ss >> width >> height;
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
            pixels.reserve(width * height);
            for (int y = 0; y < height; ++y) {
                // Find the next non-comment, non-empty line for pixel data
                do {
                    if (!std::getline(file, line)) {
                        line = ""; // Signal end of file
                        break;
                    }
                } while (line.empty() || line[0] == '#');

                if (line.empty()) break; // Reached end of file before reading all rows

                for (int x = 0; x < width; ++x) {
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

    if (width == 0 || height == 0 || pixels.empty()) {
        std::cerr << "Invalid sprite file format: " << filepath << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC,
        width,
        height
    );

    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(texture, NULL, pixels.data(), width * sizeof(uint32_t));

    return texture;
}