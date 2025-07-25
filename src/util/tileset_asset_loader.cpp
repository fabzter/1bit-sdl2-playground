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
    std::vector<std::vector<uint32_t>> tilePixelData;

    std::string line, key;
    enum class ParseSection { None, Palette, Tiles } currentSection = ParseSection::None;

    while (std::getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines entirely

        std::stringstream ss(line);
        if (line[0] == '#') {
            // If we are in the TILES section, a comment indicates a new tile definition
            if (currentSection == ParseSection::Tiles) {
                std::vector<uint32_t> currentTilePixels;
                currentTilePixels.reserve(asset->tileWidth * asset->tileHeight);

                for (int y = 0; y < asset->tileHeight; ++y) {
                    std::string pixelLine;
                    // Get the next line of pixels, skipping any blank lines in between
                    if (!std::getline(file, pixelLine)) {
                        std::cerr << "Error: Unexpected end of file while reading tile data in " << filepath << std::endl;
                        return nullptr;
                    }

                    for (int x = 0; x < asset->tileWidth; ++x) {
                        char pixel_char = (x < pixelLine.length()) ? pixelLine[x] : '0';
                        currentTilePixels.push_back(palette.count(pixel_char) ? palette.at(pixel_char) : 0xFFFF00FF); // Magenta for error
                    }
                }
                tilePixelData.push_back(currentTilePixels);
            }
            continue; // Skip processing the rest of the comment line
        }


        if (currentSection == ParseSection::Palette) {
            if (line.find("PALETTE_END") != std::string::npos) {
                currentSection = ParseSection::None;
            } else {
                char index_char;
                int r, g, b, a;
                if (ss >> index_char >> r >> g >> b >> a) {
                    palette[index_char] = (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | (static_cast<uint32_t>(g) << 8) | static_cast<uint32_t>(r);
                }
            }
        } else if (currentSection == ParseSection::Tiles) {
            if (line.find("TILES_END") != std::string::npos) {
                currentSection = ParseSection::None;
            }
            // The actual tile reading is now handled when a '#' is found.
        } else { // ParseSection::None
            ss >> key;
            if (key == "TILESET_NAME") ss >> asset->assetId;
            else if (key == "TILE_SIZE") ss >> asset->tileWidth >> asset->tileHeight;
            else if (key == "COLUMNS") ss >> asset->columns;
            else if (key == "PALETTE_BEGIN") currentSection = ParseSection::Palette;
            else if (key == "TILES_BEGIN") currentSection = ParseSection::Tiles;
        }
    }

    if (asset->tileWidth == 0 || asset->tileHeight == 0 || tilePixelData.empty() || asset->columns == 0) {
        std::cerr << "Invalid tileset file: " << filepath << " - missing critical data or tiles." << std::endl;
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