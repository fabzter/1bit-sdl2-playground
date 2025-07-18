#include "input_manager.hpp"
#include <iostream>
#include <cmath>

InputManager::InputManager() {
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Failed to initialize SDL GameController subsystem: " << SDL_GetError() << std::endl;
    }
}

InputManager::~InputManager() {
    for (auto controller : m_controllers) {
        if (controller) {
            SDL_GameControllerClose(controller);
        }
    }
    m_controllers.clear();
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void InputManager::prepareForUpdate() {
    // This is the key to single-frame event detection.
    // At the start of a new frame, the "current" state from last frame becomes the "previous" state.
    for (auto& pair : m_stateActions) {
        pair.second.was_pressed = pair.second.is_pressed;
    }
}

void InputManager::handleEvent(const SDL_Event& event) {
    std::string actionName;

    switch(event.type) {
        // --- Keyboard ---
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            auto it = m_keyActionMap.find(event.key.keysym.sym);
            if (it != m_keyActionMap.end()) {
                actionName = it->second;
                // Don't process key repeats
                if (!event.key.repeat) {
                    m_stateActions[actionName].is_pressed = (event.type == SDL_KEYDOWN);
                }
            }
            break;
        }

        // --- Controller Buttons ---
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP: {
            auto it = m_buttonActionMap.find(static_cast<SDL_GameControllerButton>(event.cbutton.button));
            if (it != m_buttonActionMap.end()) {
                actionName = it->second;
                m_stateActions[actionName].is_pressed = (event.type == SDL_CONTROLLERBUTTONDOWN);
            }
            break;
        }

            // --- Controller Axes (Axis Actions) --- (NEW)
        case SDL_CONTROLLERAXISMOTION: {
            auto it = m_axisActionMap.find(static_cast<SDL_GameControllerAxis>(event.caxis.axis));
            if (it != m_axisActionMap.end()) {
                actionName = it->second;
                float rawValue = event.caxis.value;

                // Apply dead zone and normalize the value from -32768->32767 to -1.0->1.0
                if (std::abs(rawValue) > m_joystickDeadZone) {
                    //TODO: check if it is ok that this normalization value is hardcoded
                    m_axisActions[actionName].value = rawValue / 32767.0f;
                } else {
                    m_axisActions[actionName].value = 0.0f;
                }
            }
            break;
        }

        // --- Controller Connection Management ---
        case SDL_CONTROLLERDEVICEADDED:
            openController(event.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            closeController(event.cdevice.which);
            break;
    }
}

bool InputManager::isActionPressed(const std::string& actionName) const {
    auto it = m_stateActions.find(actionName);
    return (it != m_stateActions.end()) && it->second.is_pressed;
}

bool InputManager::isActionJustPressed(const std::string& actionName) const {
    auto it = m_stateActions.find(actionName);
    // True only on the single frame it was first pressed.
    return (it != m_stateActions.end()) && it->second.is_pressed && !it->second.was_pressed;
}

bool InputManager::isActionJustReleased(const std::string& actionName) const {
    auto it = m_stateActions.find(actionName);
    // True only on the single frame it was released.
    return (it != m_stateActions.end()) && !it->second.is_pressed && it->second.was_pressed;
}


void InputManager::mapKeyToAction(SDL_Keycode key, const std::string& actionName) {
    m_keyActionMap[key] = actionName;
    m_stateActions[actionName] = {false, false}; // Ensure the action exists
}

void InputManager::mapButtonToAction(SDL_GameControllerButton button, const std::string& actionName) {
    m_buttonActionMap[button] = actionName;
    m_stateActions[actionName] = {false, false}; // Ensure the action exists
}

// --- Axis Action Implementations ---
float InputManager::getAxisValue(const std::string& actionName) const {
    auto it = m_axisActions.find(actionName);
    if (it != m_axisActions.end()) {
        return it->second.value;
    }
    return 0.0f;
}

void InputManager::mapAxisToAction(SDL_GameControllerAxis axis, const std::string& actionName) {
    m_axisActionMap[axis] = actionName;
    m_axisActions[actionName] = { 0.0f }; // Ensure the action exists
}

void InputManager::openController(int deviceIndex) {
    if (!SDL_IsGameController(deviceIndex)) return;

    SDL_GameController* controller = SDL_GameControllerOpen(deviceIndex);
    if (controller) {
        m_controllers.push_back(controller);
        std::cout << "InputManager: Opened Controller '" << SDL_GameControllerName(controller) << "'." << std::endl;
    } else {
        std::cerr << "InputManager: Could not open gamecontroller " << deviceIndex << ": " << SDL_GetError() << std::endl;
    }
}

void InputManager::closeController(int instanceId) {
    for (size_t i = 0; i < m_controllers.size(); ++i) {
        if (m_controllers[i] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_controllers[i])) == instanceId) {
            std::cout << "InputManager: Closed Controller." << std::endl;
            SDL_GameControllerClose(m_controllers[i]);
            m_controllers.erase(m_controllers.begin() + i);
            return;
        }
    }
}
