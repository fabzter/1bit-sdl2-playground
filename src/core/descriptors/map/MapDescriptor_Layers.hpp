#pragma once
#include "MapDescriptor_Foundations.hpp"

namespace TMX {

// --- Data & Chunks ---
struct Chunk {
    int x = 0, y = 0, width = 0, height = 0;
    std::vector<uint32_t> gids;
};

struct Data {
    enum class Encoding { XML, Base64, CSV };
    enum class Compression { None, Gzip, Zlib, Zstd };
    Encoding encoding = Encoding::XML;
    Compression compression = Compression::None;
    std::variant<std::vector<uint32_t>, std::vector<Chunk>> payload;
};

// --- Object Shapes ---
struct Point {};
struct Ellipse {};
struct Polygon { std::vector<Vec2f> points; };
struct Polyline { std::vector<Vec2f> points; };
struct Text {
    std::string fontFamily = "sans-serif";
    int pixelSize = 16;
    bool wrap = false;
    Color color = {0, 0, 0, 255};
    bool bold = false, italic = false, underline = false, strikeout = false;
    bool kerning = true;
    HAlign hAlign = HAlign::Left;
    VAlign vAlign = VAlign::Top;
    std::string content;
};

// --- Layer Types ---
struct Object {
    int id = 0;
    std::string name, type;
    float x = 0.0f, y = 0.0f, width = 0.0f, height = 0.0f;
    float rotation = 0.0f;
    std::optional<uint32_t> gid;
    bool visible = true;
    std::optional<std::string> templateFile;
    std::optional<Properties> properties;
    std::variant<std::monostate, Point, Ellipse, Polygon, Polyline, Text> objectShape;
};

struct ObjectGroup {
    int id = 0;
    std::string name;
    std::optional<std::string> classType;
    std::optional<Color> color;
    float opacity = 1.0f, offsetX = 0.0f, offsetY = 0.0f;
    float parallaxX = 1.0f, parallaxY = 1.0f;
    bool visible = true;
    std::optional<Color> tintColor;
    DrawOrder drawOrder = DrawOrder::TopDown;
    std::optional<Properties> properties;
    std::vector<Object> objects;
};

struct TileLayer {
    int id = 0;
    std::string name;
    std::optional<std::string> classType;
    int x = 0, y = 0, width = 0, height = 0;
    float opacity = 1.0f, offsetX = 0.0f, offsetY = 0.0f;
    float parallaxX = 1.0f, parallaxY = 1.0f;
    bool visible = true;
    std::optional<Color> tintColor;
    std::optional<Properties> properties;
    std::optional<Data> data;
};

// Forward-declare for recursive variant
struct GroupLayer;

using LayerVariant = std::variant<TileLayer, ObjectGroup, GroupLayer>;

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

} // namespace TMX
