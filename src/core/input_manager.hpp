#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>

/**
 * @struct StateAction
 * @brief Tracks the current and previous frame's state of a single action.
 * This allows for detecting single-frame events like "just pressed".
 */
struct StateAction {
    bool is_pressed = false;
    bool was_pressed = false;
};

/**
 * @struct AxisAction
 * @brief Tracks the state of an analog (float) action.
 */
struct AxisAction {
    float value = 0.0f;
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

    // --- Axis Action Methods ---
    float getAxisValue(const std::string& actionName) const;
    void mapAxisToAction(SDL_GameControllerAxis axis, const std::string& actionName);

    // --- Getters for saving configuration ---
    [[nodiscard]] const std::unordered_map<SDL_Keycode, std::string>& getKeyToActionMap() const { return m_keyActionMap; }
    [[nodiscard]] const std::unordered_map<SDL_GameControllerButton, std::string>& getButtonToActionMap() const { return m_buttonActionMap; }
    [[nodiscard]] const std::unordered_map<SDL_GameControllerAxis, std::string>& getAxisToActionMap() const { return m_axisActionMap; }

private:
    void openController(int deviceIndex);
    void closeController(int instanceId);

    // Maps a physical input to the string name of an action.
    std::unordered_map<SDL_Keycode, std::string> m_keyActionMap;
    std::unordered_map<SDL_GameControllerButton, std::string> m_buttonActionMap;
    std::unordered_map<SDL_GameControllerAxis, std::string> m_axisActionMap;
    // ddddmouse buttons, etc. here ...

    // State storage for each action type
    std::unordered_map<std::string, StateAction> m_stateActions;
    std::unordered_map<std::string, AxisAction> m_axisActions;

    // Manages connected controllers.
    std::vector<SDL_GameController*> m_controllers;
    // for handling drift
    //TODO: it must be a configurable value
    const int m_joystickDeadZone = 8000;
};
