#pragma once

#include "../core/math_types.hpp"

// Defines the physical behavior of a rigidbody in the simulation.
enum class BodyType {
    STATIC,    // Unmovable, infinite mass (e.g., walls, the ground).
    DYNAMIC,   // Movable, affected by forces and collisions (e.g., crates, enemies).
    KINEMATIC, // Movable by code, but not by forces (e.g., moving platforms).
};

/**
 * @struct RigidBodyComponent
 * @brief Stores movement properties for an entity in the physics simulation.
 */
struct RigidBodyComponent {
    Vec2f velocity{0.0f, 0.0f};
    BodyType bodyType = BodyType::DYNAMIC;

    // The mass of the object. A value of 0.0f makes it immovable (effectively infinite mass).
    float mass = 1.0f;

    // The "bounciness" of the object. 0.0 is no bounce, 1.0 is a perfect bounce.
    float restitution = 0.5f;
    
    // We can add more physics properties here in the future.
    // Vec2f acceleration{0.0f, 0.0f};
    // float friction = 0.1f;
};
