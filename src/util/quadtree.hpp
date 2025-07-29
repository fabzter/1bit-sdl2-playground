#pragma once

#include <vector>
#include <memory>
#include <array>

struct QuadtreeRect {
    int x = 0, y = 0, w = 0, h = 0;
};

/**
 * @class Quadtree
 * @brief A generic, templated Quadtree for storing any object type with rectangular bounds.
 * @tparam T The type of object/identifier to store (e.g., entt::entity, uint32_t).
 */
template <typename T>
class Quadtree {
public:
    /**
     * @brief Constructs a Quadtree.
     * @param level The depth level of this node in the tree.
     * @param bounds The rectangular boundary this node represents.
     */
    Quadtree(int level, QuadtreeRect bounds)
        : m_level(level), m_bounds(bounds) {}

    /**
     * @brief Clears the Quadtree, recursively clearing all nodes.
     * This should be called each frame before re-inserting objects.
     */
    void clear();

    /**
     * @brief Inserts an entity and its bounding box into the Quadtree.
     * @param entity The entity to insert.
     * @param entityBounds The AABB of the entity.
     */
    void insert(T object, QuadtreeRect objectBounds);

    /**
     * @brief Queries the tree and returns a list of entities that could potentially collide with the given area.
     * @param area The rectangular area to query against.
     * @param foundObjects A reference to a vector that will be filled with potential colliders.
     */
    void query(QuadtreeRect area, std::vector<T>& foundObjects);

private:
    // The maximum number of objects a node can hold before it splits.
    static constexpr int MAX_OBJECTS = 10;
    // The deepest level a node can be.
    static constexpr int MAX_LEVELS = 5;

    int m_level;
    QuadtreeRect m_bounds;
    std::vector<std::pair<T, QuadtreeRect>> m_objects;
    std::array<std::unique_ptr<Quadtree<T>>, 4> m_nodes;

    /**
     * @brief Splits the node into 4 sub-nodes.
     */
    void split();

    /**
     * @brief Determines which node an object belongs to.
     * @param rect The bounding box of the object.
     * @return The index of the child node (0-3), or -1 if it doesn't fit completely in a child and belongs to the parent.
     */
    int getIndex(QuadtreeRect rect) const;

    bool hasIntersection(const QuadtreeRect& a, const QuadtreeRect& b) const {
        return (a.x < b.x + b.w && a.x + a.w > b.x &&
                a.y < b.y + b.h && a.y + a.h > b.y);
    }
};

template <typename T>
void Quadtree<T>::clear() {
    m_objects.clear();
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i]) {
            m_nodes[i]->clear();
            m_nodes[i] = nullptr;
        }
    }
}

template <typename T>
void Quadtree<T>::split() {
    int subWidth = m_bounds.w / 2;
    int subHeight = m_bounds.h / 2;
    int x = m_bounds.x;
    int y = m_bounds.y;

    m_nodes[0] = std::make_unique<Quadtree<T>>(m_level + 1, QuadtreeRect{x + subWidth, y, subWidth, subHeight});
    m_nodes[1] = std::make_unique<Quadtree<T>>(m_level + 1, QuadtreeRect{x, y, subWidth, subHeight});
    m_nodes[2] = std::make_unique<Quadtree<T>>(m_level + 1, QuadtreeRect{x, y + subHeight, subWidth, subHeight});
    m_nodes[3] = std::make_unique<Quadtree<T>>(m_level + 1, QuadtreeRect{x + subWidth, y + subHeight, subWidth, subHeight});
}

template <typename T>
int Quadtree<T>::getIndex(QuadtreeRect rect) const {
    int index = -1;
    double verticalMidpoint = m_bounds.x + (m_bounds.w / 2.0);
    double horizontalMidpoint = m_bounds.y + (m_bounds.h / 2.0);

    bool topQuadrant = (rect.y < horizontalMidpoint && rect.y + rect.h < horizontalMidpoint);
    bool bottomQuadrant = (rect.y > horizontalMidpoint);

    if (rect.x < verticalMidpoint && rect.x + rect.w < verticalMidpoint) {
        if (topQuadrant) index = 1;
        else if (bottomQuadrant) index = 2;
    }
    else if (rect.x > verticalMidpoint) {
        if (topQuadrant) index = 0;
        else if (bottomQuadrant) index = 3;
    }
    return index;
}

template <typename T>
void Quadtree<T>::insert(T object, QuadtreeRect objectBounds) {
    if (m_nodes[0]) {
        int index = getIndex(objectBounds);
        if (index != -1) {
            m_nodes[index]->insert(object, objectBounds);
            return;
        }
    }

    m_objects.emplace_back(object, objectBounds);

    if (m_objects.size() > MAX_OBJECTS && m_level < MAX_LEVELS) {
        if (!m_nodes[0]) {
            split();
        }

        size_t i = 0;
        while (i < m_objects.size()) {
            int index = getIndex(m_objects[i].second);
            if (index != -1) {
                m_nodes[index]->insert(m_objects[i].first, m_objects[i].second);
                m_objects.erase(m_objects.begin() + i);
            } else {
                i++;
            }
        }
    }
}

template <typename T>
void Quadtree<T>::query(QuadtreeRect area, std::vector<T>& foundObjects) {
    // Check if the query area intersects with this node's bounds before proceeding
    if (!hasIntersection(area, m_bounds)) {
        return;
    }

    for (const auto& pair : m_objects) {
        // We can do an additional check here for more accuracy if needed
        if (hasIntersection(area, pair.second)) {
            foundObjects.push_back(pair.first);
        }
    }

    if (m_nodes[0]) {
        // The original logic was slightly flawed, querying all children if the area overlaps
        // the parent is simpler and more correct than trying to find a specific index.
        for (int i = 0; i < m_nodes.size(); i++) {
            m_nodes[i]->query(area, foundObjects);
        }
    }
}
