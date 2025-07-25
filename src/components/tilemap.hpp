#pragma once

#include <vector>
#include <string>

/**
 * @struct TileLayer
 * @brief Represents a single layer of a tilemap.
 */
struct TileLayer {
    int widthInTiles = 0;
    int heightInTiles = 0;
    
    // A 1D vector representing a 2D grid of tile IDs.
    // Index is calculated as: y * width + x.
    // A value of 0 means the tile is empty.
    std::vector<int> tileIds;
};

/**
 * @struct TilemapComponent
 * @brief Holds all data for a tilemap instance.
 */
struct TilemapComponent {
    int tileWidth = 0;  // in pixels
    int tileHeight = 0; // in pixels

    // The assetId of the Tileset to use for rendering this map.
    std::string tilesetAssetId;

    std::vector<TileLayer> layers;
};
