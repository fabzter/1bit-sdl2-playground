#pragma once

#include <string>

// Forward declaration to avoid including input_manager.hpp in a header
class InputManager;

/**
 * @class InputConfigLoader
 * @brief A utility class to load input bindings from a configuration file.
 */
class InputConfigLoader {
public:
    /**
     * @brief Loads input bindings from the specified file and configures the InputManager.
     * @param inputManager The InputManager instance to configure.
     * @param filepath The full path to the input configuration file.
     * @return True if loading was successful, false otherwise.
     */
    static bool loadFromFile(InputManager& inputManager, const std::string& filepath);

    /**
      * @brief Saves the current input bindings from the InputManager to a file.
      * @param inputManager The InputManager instance containing the bindings to save.
      * @param filepath The full path to the file where bindings will be saved.
      * @return True if saving was successful, false otherwise.
      */
    static bool saveToFile(const InputManager& inputManager, const std::string& filepath);
};
