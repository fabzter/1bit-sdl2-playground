#pragma once
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <cstdint>

namespace TMX {

    // Forward-declare for recursive variant
    struct Properties;

    // --- Foundational Types ---
    struct Vec2f { float x = 0.0f; float y = 0.0f; };
    struct Vec2i { int x = 0; int y = 0; };
    struct Color { uint8_t r = 0; uint8_t g = 0; uint8_t b = 0; uint8_t a = 255; };

    // --- Property System ---
    using PropertyValue = std::variant<
        bool,
        int,
        float,
        std::string,
        Color,
        Properties // For nested class properties
    >;

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

} // namespace TMX
