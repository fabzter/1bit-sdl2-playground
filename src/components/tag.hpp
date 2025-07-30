#pragma once
#include <string>

/**
 * @struct TagComponent
 * @brief Associates a string-based name with an entity.
 *
 * This component is essential for identifying entities in a way that is meaningful
 * to developers. While `entt::entity` handles are efficient for systems, they are
 * not descriptive. Attaching a `TagComponent` allows for easier debugging, logging,
* and even finding specific, unique entities by name (e.g., "Player", "MainCamera").
 *
 * The name is typically loaded from a scene file, such as a TOML or JSON definition.
 */
struct TagComponent {
    /**
     * @brief The human-readable name of the entity.
     */
    std::string name;
};
