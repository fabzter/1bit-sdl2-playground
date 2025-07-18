#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>

/**
 * @struct ActionState
 * @brief Tracks the current and previous frame's state of a single action.
 * This allows for detecting single-frame events like "just pressed".
 */
struct ActionState {
    bool is_pressed = false;
    bool was_pressed = false;
};

class InputManager {
public:
    InputManager();
    ~InputManager();

    // Called once per frame, before polling events, to update states.
    void prepareForUpdate();

    // Called for each SDL_Event from the main event loop.
    void handleEvent(const SDL_Event& event);

    // Methods to check the state of an action.
    bool isActionPressed(const std::string& actionName) const;
    bool isActionJustPressed(const std::string& actionName) const;
    bool isActionJustReleased(const std::string& actionName) const;

    // --- Configuration ---
    void mapKeyToAction(SDL_Keycode key, const std::string& actionName);
    void mapButtonToAction(SDL_GameControllerButton button, const std::string& actionName);

private:
    void openController(int deviceIndex);
    void closeController(int instanceId);

    // Maps a physical input to the string name of an action.
    std::unordered_map<SDL_Keycode, std::string> m_keyActionMap;
    std::unordered_map<SDL_GameControllerButton, std::string> m_buttonActionMap;
    // ... we could add maps for axes, mouse buttons, etc. here ...

    // Stores the current state for every known action.
    std::unordered_map<std::string, ActionState> m_actionStates;

    // Manages connected controllers.
    std::vector<SDL_GameController*> m_controllers;
};
