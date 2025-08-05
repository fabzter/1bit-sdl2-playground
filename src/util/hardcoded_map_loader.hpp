#pragma once

#include "../core/tilemap_loader.hpp"
#include "../core/descriptors/map/TMX.hpp"

// Forward-declaration
class ResourceManager;

class HardcodedMapLoader : public ITilemapLoader {
public:
    explicit HardcodedMapLoader(const TMX::Map& mapDescriptor);

    bool load(entt::registry& registry,
              entt::entity tilemapEntity,
              ResourceManager& resourceManager,
              // Note: The sourcePath parameter is ignored as we load from a C++ object.
              const std::string& sourcePath) override;

private:
    void createCollisionObjects(entt::registry& registry, const TMX::ObjectGroup& objectGroup);
    
    // A reference to the C++ map definition.
    const TMX::Map& m_mapDescriptor;
};
