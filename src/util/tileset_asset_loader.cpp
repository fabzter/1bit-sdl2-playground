#include "tileset_asset_loader.hpp"
#include "text_asset_parser.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>

std::unique_ptr<TilesetAsset> TilesetAssetLoader::loadFromFile(SDL_Renderer* renderer, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open tileset file: " << filepath << std::endl;
        return nullptr;
    }

    auto asset = std::make_unique<TilesetAsset>();
    std::vector<std::vector<uint32_t>> tilePixelData;
    TextAssetParser::PaletteMap palette;

    std::string line;
    enum class ParseSection { None, Tiles } currentSection = ParseSection::None;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Handle tile comments only when in the TILES section
        if (line[0] == '#') {
            if (currentSection == ParseSection::Tiles) {
                // This comment marks the beginning of a new tile's pixel data
                tilePixelData.push_back(TextAssetParser::parsePixelBlock(file, asset->tileWidth, asset->tileHeight, palette));
            }
            // Otherwise, it's a generic comment, so we skip it.
            continue;
        }

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "TILESET_NAME") ss >> asset->assetId;
        else if (key == "TILE_SIZE") ss >> asset->tileWidth >> asset->tileHeight;
        else if (key == "COLUMNS") ss >> asset->columns;
        else if (key == "PALETTE_BEGIN") palette = TextAssetParser::parsePalette(file);
        else if (key == "TILES_BEGIN") currentSection = ParseSection::Tiles;
        else if (key == "TILES_END") currentSection = ParseSection::None;
    }

    if (asset->tileWidth == 0 || asset->tileHeight == 0 || tilePixelData.empty() || asset->columns == 0) {
        std::cerr << "Invalid or empty tileset file: " << filepath << std::endl;
        return nullptr;
    }

    // --- Texture Atlas creation logic remains the same ---
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
        if (tilePixelData[i].size() != (size_t)asset->tileWidth * asset->tileHeight) continue;
        int tileCol = i % asset->columns;
        int tileRow = i / asset->columns;
        SDL_Rect destRect = { tileCol * asset->tileWidth, tileRow * asset->tileHeight, asset->tileWidth, asset->tileHeight };
        SDL_UpdateTexture(rawTexture, &destRect, tilePixelData[i].data(), asset->tileWidth * sizeof(uint32_t));
    }

    asset->textureAtlas.reset(rawTexture);
    return asset;
}
