#include "text_asset_parser.hpp"
#include <sstream>
#include <iostream>

TextAssetParser::PaletteMap TextAssetParser::parsePalette(std::ifstream& file) {
    PaletteMap palette;
    std::string line;
    while(std::getline(file, line) && line.find("PALETTE_END") == std::string::npos) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream pss(line);
        char index_char;
        int r, g, b, a;
        if (pss >> index_char >> r >> g >> b >> a) {
            uint32_t color = (static_cast<uint32_t>(a) << 24) |
                             (static_cast<uint32_t>(b) << 16) |
                             (static_cast<uint32_t>(g) << 8)  |
                              static_cast<uint32_t>(r);
            palette[index_char] = color;
        }
    }
    return palette;
}

std::vector<uint32_t> TextAssetParser::parsePixelBlock(std::ifstream& file, int width, int height, const PaletteMap& palette) {
    std::vector<uint32_t> pixels;
    pixels.reserve(width * height);
    std::string line;

    for (int y = 0; y < height; ++y) {
        // Get the next non-empty, non-comment line
        do {
            if (!std::getline(file, line)) {
                std::cerr << "TextAssetParser Error: Unexpected end of file while reading pixel data." << std::endl;
                // Return whatever we have, the caller should validate the size.
                return pixels;
            }
        } while (line.empty() || line[0] == '#');

        for (int x = 0; x < width; ++x) {
            char pixel_char = (x < line.length()) ? line[x] : '0';
            auto it = palette.find(pixel_char);
            if (it != palette.end()) {
                pixels.push_back(it->second);
            } else {
                pixels.push_back(0xFFFF00FF); // Magenta for error
            }
        }
    }
    return pixels;
}
