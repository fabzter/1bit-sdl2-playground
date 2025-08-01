#pragma once
#include "MapDescriptor_Foundations.hpp"

namespace TMX {

    // Forward-declare for recursive definition
    struct ObjectGroup;

    // --- Image & Animation ---
    struct Image {
        std::optional<std::string> format;
        std::string source;
        std::optional<Color> transparentColor;
        std::optional<int> width;
        std::optional<int> height;
    };

    struct AnimationFrame {
        int tileId = 0;
        int duration = 0;
    };

    struct Animation {
        std::vector<AnimationFrame> frames;
    };

    // --- Tile & Tileset ---
    struct TileOffset { int x = 0; int y = 0; };

    struct Grid {
        Orientation orientation = Orientation::Orthogonal;
        int width = 0;
        int height = 0;
    };

    struct Tile {
        int id = 0;
        std::optional<std::string> type;
        std::optional<Properties> properties;
        std::optional<Image> image;
        std::optional<ObjectGroup> objectGroup; // For collision editor
        std::optional<Animation> animation;
    };

    struct Tileset {
        int firstGid = 1;
        std::optional<std::string> source;
        std::string name;
        std::optional<std::string> classType;
        int tileWidth = 0;
        int tileHeight = 0;
        int spacing = 0;
        int margin = 0;
        int tileCount = 0;
        int columns = 0;
        ObjectAlignment objectAlignment = ObjectAlignment::Unspecified;
        TileRenderSize tileRenderSize = TileRenderSize::Tile;
        FillMode fillMode = FillMode::Stretch;

        std::optional<TileOffset> tileOffset;
        std::optional<Grid> grid;
        std::optional<Properties> properties;
        std::optional<Image> image;
        std::vector<Tile> tiles;
    };

} // namespace TMX
