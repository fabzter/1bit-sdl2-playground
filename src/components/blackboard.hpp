#pragma once

#include <string>
#include <unordered_map>
#include <any>

/**
 * @file blackboard.hpp
* @details
 * The Blackboard pattern provides a centralized, shared data store that various
 * systems can access to communicate and share information without being directly
 * coupled to one another. Think of it as a physical blackboard where different
 * experts (in this case, systems) can write down findings, and other experts can read them
 * to contribute to solving a larger problem, all without needing to know who wrote what.
 *
 * In this Entity-Component-System (ECS) context, the `BlackboardComponent`
 * serves as this shared space for a single entity. It uses a map with string
 * keys, allowing systems to store and retrieve data of any type (`std::any`)
 * by a well-known name.
 *
 * ### Purpose & Benefits
 * - **Decoupling:** A system that sets a "health" value doesn't need to know
 *   about the UI system that reads it. They only need to agree on the key "health".
 *   This makes systems more modular and easier to maintain or replace.
 * - **Flexibility:** Ideal for dynamic or heterogeneous data that doesn't fit
 *   into a rigid component structure, such as temporary status effects, AI state,
 *   or quest flags.
 * - **Data-Driven Design:** Encourages systems to react to data changes rather
 *   than being called directly by other systems.
 */

/**
 * @struct BlackboardComponent
 * @brief A generic, data-driven component for storing an entity's state.
 *
 * Systems can write to or read from the blackboard using string keys. This
 * decouples systems by allowing them to communicate through this shared data
 * store without needing to know about each other. It's the C++/ECS equivalent
 * of the parameter dictionaries found in engines like Godot and Unity.
 */
struct BlackboardComponent {
    std::unordered_map<std::string, std::any> values;
};