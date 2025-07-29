#pragma once

#include <vector>
#include <unordered_map>
#include <fstream>
#include <SDL2/SDL_pixels.h>

/**
 * @class TextAssetParser
 * @brief A utility class with static methods to parse common patterns in our custom text-based asset files.
 * This class is not meant to be instantiated. It centralizes parsing logic to avoid code duplication.
 */
class TextAssetParser {
public:
    // A map from a character in the file (e.g., '1') to its 32-bit RGBA color.
    using PaletteMap = std::unordered_map<char, uint32_t>;

    /**
     * @brief Parses a PALETTE_BEGIN/PALETTE_END block from the file stream.
     * @param file The active input file stream.
     * @param format
     * @return A map containing the parsed palette data.
     */
    static PaletteMap parsePalette(std::ifstream& file, const SDL_PixelFormat* format);

    /**
     * @brief Parses a block of pixel data from the file stream until a frame delimiter is found.
     * @param file The active input file stream.
     * @param width The expected width of the frame/tile.
     * @param height The expected height of the frame/tile.
     * @param palette The palette to use for converting characters to colors.
     * @return A vector of pixel data for a single frame/tile.
     */
    static std::vector<uint32_t> parsePixelBlock(std::ifstream& file, int width, int height, const PaletteMap& palette);
};
