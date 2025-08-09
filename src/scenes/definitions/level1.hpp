#pragma once
#include "../../core/descriptors/map/TMX.hpp"

namespace AssetDefinitions {

// This is our C++ representation of 'level1.tmx' using the final TMX API.
inline const TMX::Map Level1Map = []{
    TMX::Map map;
    map.version = "1.10";
    map.tiledVersion = "1.11.2";
    map.orientation = TMX::Orientation::Orthogonal;
    map.renderOrder = TMX::RenderOrder::RightDown;
    map.width = 10;
    map.height = 8;
    map.tileWidth = 16;
    map.tileHeight = 16;
    map.nextLayerId = 3;
    map.nextObjectId = 9;

    // --- Tilesets ---
    map.tilesets.emplace_back(TMX::Tileset{
        .firstGid = 1,
        .name = "ground",
        .tileWidth = 16,
        .tileHeight = 16,
        .columns = 2,
        .image = TMX::Image{ .source = "ground.tileset", .width = 32, .height = 16 }
    });

    // --- Tile Layer ---
    TMX::TileLayer groundLayer;
    groundLayer.id = 1;
    groundLayer.name = "GroundLayer";
    groundLayer.width = 19;
    groundLayer.height = 8;
    groundLayer.data = TMX::Data{
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
    };
    map.layers.emplace_back(std::move(groundLayer));

    // --- Object Layer ---
    TMX::ObjectGroup collisionGroup;
    collisionGroup.id = 2;
    collisionGroup.name = "Collisions";

    // Create a helper lambda to generate the properties for each object
    auto make_collision_props = []() {
        TMX::Properties props;
        props.list.emplace_back(TMX::Property{"collisionLayerName", "string", std::nullopt, "WORLD"});
        props.list.emplace_back(TMX::Property{"collisionMaskNames", "string", std::nullopt, "PLAYER"});
        return props;
    };

    collisionGroup.objects.emplace_back(TMX::Object{ .id = 1, .position = {0, 0}, .size = {160, 16}, .properties = make_collision_props() });
    collisionGroup.objects.emplace_back(TMX::Object{ .id = 2, .position = {0, 112}, .size = {160, 16}, .properties = make_collision_props() });
    collisionGroup.objects.emplace_back(TMX::Object{ .id = 3, .position = {0, 16}, .size = {16, 96}, .properties = make_collision_props() });
    collisionGroup.objects.emplace_back(TMX::Object{ .id = 4, .position = {144, 16}, .size = {16, 32}, .properties = make_collision_props() });
    collisionGroup.objects.emplace_back(TMX::Object{ .id = 5, .position = {144, 80}, .size = {16, 32}, .properties = make_collision_props() });
    collisionGroup.objects.emplace_back(TMX::Object{ .id = 6, .position = {160, 0}, .size = {144, 16}, .properties = make_collision_props() });
    collisionGroup.objects.emplace_back(TMX::Object{ .id = 7, .position = {160, 111.855f}, .size = {144, 16}, .properties = make_collision_props() });
    collisionGroup.objects.emplace_back(TMX::Object{ .id = 8, .position = {287.523f, 16}, .size = {16, 96}, .properties = make_collision_props() });

    map.layers.emplace_back(std::move(collisionGroup));

    return map;
}();

    // C++ representation of the entities from 'level1.toml'
inline const SceneDescriptor Level1Scene = {
    .entities = {
        // --- Crate ---
        {
            .name = "CRATE",
            .components = {
                TransformDescriptor{ .position = {100.0f, 64.0f}, .scale = {2.0f, 2.0f} },
                SpriteDescriptor{ .assetId = "player", .isAnimated = true, .orderInLayer = 9 },
                ColliderDescriptor{ .size = {14.0f, 14.0f}, .layer = 1, .mask = 3 },
                RigidBodyDescriptor{ .bodyType = RigidBodyDescriptor::Dynamic, .mass = 10.0f, .restitution = 0.2f, .damping = 0.9f }
            }
        },
        // --- Coin ---
        {
            .name = "Coin",
            .components = {
                TransformDescriptor{ .position = {40.0f, 40.0f}, .scale = {2.0f, 2.0f} },
                SpriteDescriptor{ .assetId = "player", .isAnimated = true, .orderInLayer = 5 },
                ColliderDescriptor{ .size = {10.0f, 10.0f}, .layer = 1, .mask = 2, .isStatic = true, .isTrigger = true },
                BehaviorDescriptor{ .type = "collectible" }
            }
        },
        // --- Player ---
        {
            .name = "Player",
            .components = {
                TransformDescriptor{ .position = {80.0f, 64.0f}, .scale = {2.0f, 2.0f} },
                RigidBodyDescriptor{ .bodyType = RigidBodyDescriptor::Kinematic, .mass = 1.0f, .restitution = 0.5f, .damping = 0.9f },
                ColliderDescriptor{ .size = {10.0f, 14.0f}, .layer = 2, .mask = 1 },
                SpriteDescriptor{ .assetId = "player", .isAnimated = true, .orderInLayer = 10 },
                PlayerControlTag{},
                MovementDescriptor{ .speed = 200.0f },
                IntentTag{},
                BlackboardTag{},
                StateMachineDescriptor{
                    .initialState = "idle",
                    .states = {
                        {"idle", {"simple", "idle"}},
                        {"walk", {"simple", "walk"}}
                    },
                    .transitions = {
                        { "idle", "walk", {{"isMoving", true}} },
                        { "walk", "idle", {{"isMoving", false}} }
                    }
                }
            }
        },
        // --- Camera ---
        {
            .name = "Camera",
            .components = {
                TransformDescriptor{},
                CameraTag{},
                BlackboardTag{}
            },
            .blackboard = {
                {"cameraTarget", "Player"}, // Value is a string for now, will be resolved to an entity
                {"followSpeed", 12.0f},
                {"deadZoneRadius", 2.0f}
            }
        }
    }
};

} // namespace AssetDefinitions
