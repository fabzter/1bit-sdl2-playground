#include "tileset_asset_loader.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <cmath> // For std::ceil

std::unique_ptr<TilesetAsset> TilesetAssetLoader::loadFromFile(SDL_Renderer* renderer, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open tileset file: " << filepath << std::endl;
        return nullptr;
    }

    auto asset = std::make_unique<TilesetAsset>();
    std::unordered_map<char, uint32_t> palette;
    std::vector<std::vector<uint32_t>> tilePixelData; // Stores pixels for each tile separately

    std::string line, key;

    // A section parser, similar to the sprite loader
    enum class ParseSection { None, Palette, Tiles };
    ParseSection currentSection = ParseSection::None;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        if (currentSection == ParseSection::None) {
            ss >> key;
            if (key == "TILESET_NAME") ss >> asset->assetId;
            else if (key == "TILE_SIZE") ss >> asset->tileWidth >> asset->tileHeight;
            else if (key == "COLUMNS") ss >> asset->columns;
            else if (key == "PALETTE_BEGIN") currentSection = ParseSection::Palette;
            else if (key == "TILES_BEGIN") currentSection = ParseSection::Tiles;
        } else if (currentSection == ParseSection::Palette) {
            if (line.find("PALETTE_END") != std::string::npos) {
                currentSection = ParseSection::None;
                continue;
            }
            char index_char;
            int r, g, b, a;
            if (ss >> index_char >> r >> g >> b >> a) {
                palette[index_char] = (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | (static_cast<uint32_t>(g) << 8) | static_cast<uint32_t>(r);
            }
        } else if (currentSection == ParseSection::Tiles) {
            if (line.find("TILES_END") != std::string::npos) {
                currentSection = ParseSection::None;
                continue;
            }
            // Each line inside the TILES section is a row of pixels for the current tile
            std::vector<uint32_t> pixels;
            pixels.reserve(asset->tileWidth * asset->tileHeight);
            
            // This line is the first row of a new tile
            for (int y = 0; y < asset->tileHeight; ++y) {
                 for (int x = 0; x < asset->tileWidth; ++x) {
                    char pixel_char = (x < line.length()) ? line[x] : '0';
                    pixels.push_back(palette.count(pixel_char) ? palette.at(pixel_char) : 0xFFFF00FF); // Magenta for error
                }
                if (y < asset->tileHeight - 1) { // Don't read past the last row of the tile
                    std::getline(file, line);
                }
            }
            tilePixelData.push_back(pixels);
        }
    }

    if (asset->tileWidth == 0 || asset->tileHeight == 0 || tilePixelData.empty() || asset->columns == 0) {
        std::cerr << "Invalid tileset file: " << filepath << " - missing critical data." << std::endl;
        return nullptr;
    }

    asset->tileCount = tilePixelData.size();
    const int rows = static_cast<int>(std::ceil(static_cast<float>(asset->tileCount) / asset->columns));
    const int atlasWidth = asset->columns * asset->tileWidth;
    const int atlasHeight = rows * asset->tileHeight;

    SDL_Texture* rawTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, atlasWidth, atlasHeight);
    if (!rawTexture) {
        std::cerr << "Failed to create texture atlas for tileset: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_SetTextureBlendMode(rawTexture, SDL_BLENDMODE_BLEND);

    for (size_t i = 0; i < tilePixelData.size(); ++i) {
        int tileCol = i % asset->columns;
        int tileRow = i / asset->columns;
        SDL_Rect destRect = { tileCol * asset->tileWidth, tileRow * asset->tileHeight, asset->tileWidth, asset->tileHeight };
        SDL_UpdateTexture(rawTexture, &destRect, tilePixelData[i].data(), asset->tileWidth * sizeof(uint32_t));
    }

    asset->textureAtlas.reset(rawTexture);
    return asset;
}
