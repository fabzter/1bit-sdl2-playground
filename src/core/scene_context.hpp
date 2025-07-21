#pragma once

#include <string>
#include <unordered_map>
#include <any>

// The SceneContext is a map that can hold any kind of data, identified by a string key.
using SceneContext = std::unordered_map<std::string, std::any>;