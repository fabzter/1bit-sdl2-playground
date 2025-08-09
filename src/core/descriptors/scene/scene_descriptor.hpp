#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <any>
#include <unordered_map>
#include "../../math_types.hpp"

// --- Component Descriptors ---
// Each struct here mirrors a component in our engine.

struct TransformDescriptor {
    Vec2f position{0.0f, 0.0f};
    Vec2f scale{1.0f, 1.0f};
};

struct SpriteDescriptor {
    std::string assetId;
    bool isAnimated = false;
    int16_t sortingLayer = 0;
    int16_t orderInLayer = 0;
};

struct RigidBodyDescriptor {
    enum BodyType { Static, Dynamic, Kinematic };
    BodyType bodyType = Dynamic;
    float mass = 1.0f;
    float restitution = 0.5f;
    float damping = 0.98f;
};

struct ColliderDescriptor {
    Vec2f size{0.0f, 0.0f};
    Vec2f offset{0.0f, 0.0f};
    uint32_t layer = 0;
    uint32_t mask = 0;
    bool isStatic = false;
    bool isTrigger = false;
};

struct MovementDescriptor {
    float speed = 0.0f;
};

struct BehaviorDescriptor {
    std::string type;
};

struct TransitionConditionDescriptor {
    std::string blackboardKey;
    bool expectedValue;
};

struct TransitionDescriptor {
    std::string from;
    std::string to;
    std::vector<TransitionConditionDescriptor> conditions;
};

struct StateDescriptor {
    std::string type; // e.g., "simple"
    std::string animation;
};

struct StateMachineDescriptor {
    std::string initialState;
    std::unordered_map<std::string, StateDescriptor> states;
    std::vector<TransitionDescriptor> transitions;
};

struct PlayerControlTag{};

struct IntentTag{};

struct CameraTag{};

struct BlackboardTag{};

// --- Component Variant ---
// A variant to hold any possible component descriptor.
using ComponentDescriptorVariant = std::variant<
    TransformDescriptor,
    SpriteDescriptor,
    RigidBodyDescriptor,
    ColliderDescriptor,
    MovementDescriptor,
    BehaviorDescriptor,
    StateMachineDescriptor,
    // Tag components can be represented by their presence.
    // Blackboard is handled separately due to its dynamic nature.
    PlayerControlTag,
    IntentTag,
    CameraTag,
    BlackboardTag
>;

// --- Entity Descriptor ---
struct EntityDescriptor {
    std::string name;
    std::vector<ComponentDescriptorVariant> components;
    std::unordered_map<std::string, std::any> blackboard;
};

// --- Scene Descriptor ---
struct SceneDescriptor {
    std::vector<EntityDescriptor> entities;
};
