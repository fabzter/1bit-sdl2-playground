#pragma once
#include "../core/descriptors/MapDescriptor_Main.hpp"

namespace AssetDefinitions {

// C++ representation of 'level1.tmx'
inline const TMX::Map Level1Map = {
    .version = "1.10",
    .tiledVersion = "1.11.2",
    .orientation = TMX::Orientation::Orthogonal,
    .renderOrder = TMX::RenderOrder::RightDown,
    .width = 10,
    .height = 8,
    .tileWidth = 16,
    .tileHeight = 16,
    .nextLayerId = 3,
    .nextObjectId = 9,
    .tilesets = {
        {
            .firstGid = 1,
            .name = "ground",
            .tileWidth = 16,
            .tileHeight = 16,
            .columns = 2,
            .image = TMX::Image{ .source = "ground.tileset", .width = 32, .height = 16 }
        }
    },
    .layers = {
        TMX::TileLayer{
            .id = 1,
            .name = "GroundLayer",
            .width = 19, .height = 8,
            .data = TMX::Data{
                .encoding = TMX::Data::Encoding::CSV,
                .payload = std::vector<uint32_t>{
                    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
                    2,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,2,
                    2,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,2,
                    2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
                    2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
                    2,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,2,
                    2,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,2,
                    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
                }
            }
        },
        TMX::ObjectGroup{
            .id = 2,
            .name = "Collisions",
            .objects = {
                { .id = 1, .x = 0, .y = 0, .width = 160, .height = 16, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} },
                { .id = 2, .x = 0, .y = 112, .width = 160, .height = 16, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} },
                { .id = 3, .x = 0, .y = 16, .width = 16, .height = 96, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} },
                { .id = 4, .x = 144, .y = 16, .width = 16, .height = 32, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} },
                { .id = 5, .x = 144, .y = 80, .width = 16, .height = 32, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} },
                { .id = 6, .x = 160, .y = 0, .width = 144, .height = 16, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} },
                { .id = 7, .x = 160, .y = 111.855f, .width = 144, .height = 16, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} },
                { .id = 8, .x = 287.523f, .y = 16, .width = 16, .height = 96, .properties = {{{{"collisionLayerName", std::nullopt, std::nullopt, "WORLD"}}, {{"collisionMaskNames", std::nullopt, std::nullopt, "PLAYER"}}}} }
            }
        }
    }
};

} // namespace AssetDefinitions