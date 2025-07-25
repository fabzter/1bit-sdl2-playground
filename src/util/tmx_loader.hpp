#pragma once

#include "../core/tilemap_loader.hpp"

class TmxLoader : public ITilemapLoader {
public:
    bool load(entt::registry& registry,
              entt::entity tilemapEntity,
              ResourceManager& resourceManager,
              const std::string& sourcePath) override;
};
