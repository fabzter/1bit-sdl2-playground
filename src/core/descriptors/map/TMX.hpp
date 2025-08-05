#pragma once

#include "../../math_types.hpp"
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <cstdint>
#include <memory>


namespace TMX {

    // --- Forward Declarations to Break Recursive Dependencies ---
    struct Properties;
    struct ObjectGroup;
    struct GroupLayer;

    // Represents an RGBA color, matching the #AARRGGBB format.
    struct Color { uint8_t r = 0; uint8_t g = 0; uint8_t b = 0; uint8_t a = 255; };

    // --- Property System ---
    using PropertyValue = std::variant<
    bool,
    int,
    float,
    std::string,
    Color,
    std::unique_ptr<Properties> // Use pointer to break recursion
>;

    // Corresponds to <property>
    struct Property {
        std::string name;
        std::optional<std::string> type;
        std::optional<std::string> propertyType;
        PropertyValue value;
    };

    struct Properties {
        std::vector<Property> list;
    };

    // --- Enums ---
    enum class Orientation { Orthogonal, Isometric, Staggered, Hexagonal };
    enum class RenderOrder { RightDown, RightUp, LeftDown, LeftUp };
    enum class StaggerAxis { X, Y };
    enum class StaggerIndex { Even, Odd };
    enum class ObjectAlignment { Unspecified, TopLeft, Top, TopRight, Left, Center, Right, BottomLeft, Bottom, BottomRight };
    enum class TileRenderSize { Tile, Grid };
    enum class FillMode { Stretch, PreserveAspectFit };
    enum class DrawOrder { Index, TopDown };
    enum class HAlign { Left, Center, Right, Justify };
    enum class VAlign { Top, Center, Bottom };

    // --- Image, Animation, and Tile Data ---
    struct Data; // Forward-declare for Image

    // Corresponds to the <image> element.
    struct Image {
        std::optional<std::string> format;
        std::string source;
        std::optional<Color> transparentColor;
        std::optional<int> width;
        std::optional<int> height;
        std::optional<std::unique_ptr<Data>> data; // For embedded image data
    };

    // Corresponds to the <frame> element within an <animation>.
    struct AnimationFrame {
        int tileId = 0;
        int duration = 0;
    };

    struct Animation {
        std::vector<AnimationFrame> frames;
    };

    // Corresponds to <tileoffset>
    struct TileOffset { int x = 0; int y = 0; };

    // Corresponds to <grid>
    struct Grid {
        Orientation orientation = Orientation::Orthogonal;
        int width = 0;
        int height = 0;
    };

    // Corresponds to <tile> within a <tileset>
    struct Tile {
        int id = 0;
        std::optional<std::string> type; // The "class" of the tile
        std::optional<std::string> terrain_deprecated; // Deprecated in favor of wangsets
        float probability = 0.0f;
        std::optional<Properties> properties;
        std::optional<Image> image;
        std::optional<std::unique_ptr<ObjectGroup>> objectGroup; //For collision editor. Use pointer to break recursion
        std::optional<Animation> animation;
    };

// --- Terrain (Deprecated), Wangsets & Transformations ---
    struct Terrain {
        std::string name;
        int tile = -1;
        std::optional<Properties> properties;
    };

    struct WangColor {
        std::string name;
        std::optional<std::string> classType;
        Color color;
        int tile = -1;
        float probability = 0.0f;
        std::optional<Properties> properties;
    };

    struct WangTile {
        int tileId = 0;
        std::vector<uint8_t> wangId; // List of corner/edge color indexes
    };

    struct WangSet {
        std::string name;
        std::optional<std::string> classType;
        int tile = -1;
        std::vector<WangColor> colors;
        std::vector<WangTile> tiles;
        std::optional<Properties> properties;
    };

    struct Transformations {
        bool hFlip = false;
        bool vFlip = false;
        bool rotate = false;
        bool preferUntransformed = false;
    };

    // --- Tileset Definition ---
    // Corresponds to <tileset>
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
        std::vector<Terrain> terrainTypes_deprecated; // Deprecated
        std::vector<WangSet> wangsets;
        std::optional<Transformations> transformations;
    };

    // --- Data & Chunks ---
    // Corresponds to <data>
    struct Chunk {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        std::vector<uint32_t> gids;
    };

    // Corresponds to <data>
    struct Data {
        enum class Encoding { XML, Base64, CSV };
        enum class Compression { None, Gzip, Zlib, Zstd };

        Encoding encoding = Encoding::XML;
        Compression compression = Compression::None;
        std::variant<std::vector<uint32_t>, std::vector<Chunk>> payload;
    };

    // --- Object Shapes ---
    // Corresponds to <object> shapes
    struct Point {};
    struct Ellipse {};
    struct Polygon { std::vector<Vec2f> points; };
    struct Polyline { std::vector<Vec2f> points; };
    struct Text {
        std::string fontFamily = "sans-serif";
        int pixelSize = 16;
        bool wrap = false;
        Color color = {0, 0, 0, 255};
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool strikeout = false;
        bool kerning = true;
        HAlign hAlign = HAlign::Left;
        VAlign vAlign = VAlign::Top;
        std::string content;
    };

    // --- Layer Types ---
    // Corresponds to <object> in an <objectgroup>
    struct Object {
        int id = 0;
        std::string name;
        std::string type; // The "class" of the object
        Vec2f position{0.0f, 0.0f};
        Vec2f size{0.0f, 0.0f};
        float rotation = 0.0f;
        std::optional<uint32_t> gid;
        bool visible = true;
        std::optional<std::string> templateFile;
        std::optional<Properties> properties;
        std::variant<std::monostate, struct Point, struct Ellipse, struct Polygon, struct Polyline, struct Text> objectShape;
    };

    // Corresponds to <objectgroup>
    struct ObjectGroup {
        int id = 0;
        std::string name;
        std::optional<std::string> classType;
        std::optional<Color> color;
        float opacity = 1.0f;
        bool visible = true;
        std::optional<Color> tintColor;
        float offsetX = 0.0f, offsetY = 0.0f;
        float parallaxX = 1.0f, parallaxY = 1.0f;
        DrawOrder drawOrder = DrawOrder::TopDown;
        std::optional<Properties> properties;
        std::vector<Object> objects;
    };

    struct ImageLayer {
        int id = 0;
        std::string name;
        std::optional<std::string> classType;
        float offsetX = 0.0f, offsetY = 0.0f;
        float parallaxX = 1.0f, parallaxY = 1.0f;
        float opacity = 1.0f;
        bool visible = true;
        bool repeatX = false, repeatY = false;
        std::optional<Color> tintColor;
        std::optional<Properties> properties;
        std::optional<Image> image;
    };

    struct TileLayer {
        int id = 0;
        std::string name;
        std::optional<std::string> classType;
        int x = 0, y = 0;
        int width = 0, height = 0;
        float opacity = 1.0f;
        bool visible = true;
        float offsetX = 0.0f, offsetY = 0.0f;
        float parallaxX = 1.0f, parallaxY = 1.0f;
        std::optional<Color> tintColor;
        std::optional<Properties> properties;
        std::optional<Data> data;
    };

    using LayerVariant = std::variant<TileLayer, ObjectGroup, ImageLayer, std::unique_ptr<GroupLayer>>;

    struct GroupLayer {
        int id = 0;
        std::string name;
        std::optional<std::string> classType;
        float offsetX = 0.0f, offsetY = 0.0f;
        float parallaxX = 1.0f, parallaxY = 1.0f;
        float opacity = 1.0f;
        bool visible = true;
        std::optional<Color> tintColor;
        std::optional<Properties> properties;
        std::vector<LayerVariant> layers;
    };

    // --- Top-Level Map ---
    struct Map {
        std::string version = "1.10";
        std::optional<std::string> tiledVersion;
        std::optional<std::string> classType;
        Orientation orientation = Orientation::Orthogonal;
        RenderOrder renderOrder = RenderOrder::RightDown;
        int compressionLevel = -1;
        int width = 0, height = 0;
        int tileWidth = 0, tileHeight = 0;
        std::optional<int> hexSideLength;
        std::optional<StaggerAxis> staggerAxis;
        std::optional<StaggerIndex> staggerIndex;
        Vec2f parallaxOrigin = {0.0f, 0.0f};
        std::optional<Color> backgroundColor;
        std::optional<int> nextLayerId;
        std::optional<int> nextObjectId;
        bool infinite = false;

        std::optional<Properties> properties;
        // std::optional<EditorSettings> editorSettings; // Not relevant for runtime
        std::vector<Tileset> tilesets;
        std::vector<LayerVariant> layers;
    };

} // namespace TMX
