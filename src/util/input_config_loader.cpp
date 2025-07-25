#include "input_config_loader.hpp"
#include "../core/input_manager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string_view>
#include <algorithm>

namespace {
// Helper to trim whitespace from both ends of a string_view
std::string_view trim(std::string_view str) {
    const auto strBegin = str.find_first_not_of(" \t");
    if (strBegin == std::string_view::npos) return ""; // no content
    const auto strEnd = str.find_last_not_of(" \t");
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

// Helper to convert string to uppercase for case-insensitive matching
std::string toUpper(std::string_view sv) {
    std::string s(sv);
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return s;
}

// Static maps to convert strings to SDL enums. Populated once on first use.
std::unordered_map<std::string, SDL_Keycode> keyMap;
std::unordered_map<std::string, SDL_GameControllerButton> buttonMap;
std::unordered_map<std::string, SDL_GameControllerAxis> axisMap;

    // Reverse maps for saving
std::unordered_map<SDL_Keycode, std::string> keyNameMap;
std::unordered_map<SDL_GameControllerButton, std::string> buttonNameMap;
std::unordered_map<SDL_GameControllerAxis, std::string> axisNameMap;

void initializeMaps() {
    if (!keyMap.empty()) return; // Already initialized

    // Keyboard Keys (add more as needed)
    keyMap["A"] = SDLK_a; keyMap["B"] = SDLK_b; keyMap["C"] = SDLK_c; keyMap["D"] = SDLK_d;
    keyMap["E"] = SDLK_e; keyMap["F"] = SDLK_f; keyMap["G"] = SDLK_g; keyMap["H"] = SDLK_h;
    keyMap["I"] = SDLK_i; keyMap["J"] = SDLK_j; keyMap["K"] = SDLK_k; keyMap["L"] = SDLK_l;
    keyMap["M"] = SDLK_m; keyMap["N"] = SDLK_n; keyMap["O"] = SDLK_o; keyMap["P"] = SDLK_p;
    keyMap["Q"] = SDLK_q; keyMap["R"] = SDLK_r; keyMap["S"] = SDLK_s; keyMap["T"] = SDLK_t;
    keyMap["U"] = SDLK_u; keyMap["V"] = SDLK_v; keyMap["W"] = SDLK_w; keyMap["X"] = SDLK_x;
    keyMap["Y"] = SDLK_y; keyMap["Z"] = SDLK_z;
    keyMap["SPACE"] = SDLK_SPACE;
    keyMap["UP"] = SDLK_UP; keyMap["DOWN"] = SDLK_DOWN;
    keyMap["LEFT"] = SDLK_LEFT; keyMap["RIGHT"] = SDLK_RIGHT;

    keyMap["F1"] = SDLK_F1; keyMap["F2"] = SDLK_F2; keyMap["F3"] = SDLK_F3;
    keyMap["F4"] = SDLK_F4; keyMap["F5"] = SDLK_F5; keyMap["F6"] = SDLK_F6;
    keyMap["F7"] = SDLK_F7; keyMap["F8"] = SDLK_F8; keyMap["F9"] = SDLK_F9;
    keyMap["F10"] = SDLK_F10; keyMap["F11"] = SDLK_F11; keyMap["F12"] = SDLK_F12;

    // Populate reverse key map
    for (const auto& pair : keyMap) {
        keyNameMap[pair.second] = pair.first;
    }

    // Controller Buttons
    buttonMap["A"] = SDL_CONTROLLER_BUTTON_A;
    buttonMap["B"] = SDL_CONTROLLER_BUTTON_B;
    buttonMap["X"] = SDL_CONTROLLER_BUTTON_X;
    buttonMap["Y"] = SDL_CONTROLLER_BUTTON_Y;
    buttonMap["BACK"] = SDL_CONTROLLER_BUTTON_BACK;
    buttonMap["GUIDE"] = SDL_CONTROLLER_BUTTON_GUIDE;
    buttonMap["START"] = SDL_CONTROLLER_BUTTON_START;
    buttonMap["LEFTSTICK"] = SDL_CONTROLLER_BUTTON_LEFTSTICK;
    buttonMap["RIGHTSTICK"] = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    buttonMap["LEFTSHOULDER"] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    buttonMap["RIGHTSHOULDER"] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    buttonMap["DPAD_UP"] = SDL_CONTROLLER_BUTTON_DPAD_UP;
    buttonMap["DPAD_DOWN"] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    buttonMap["DPAD_LEFT"] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    buttonMap["DPAD_RIGHT"] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;

    // populate reverse button map
    for (const auto& pair : buttonMap) {
        buttonNameMap[pair.second] = pair.first;
    }

    // Controller Axes
    axisMap["LEFTX"] = SDL_CONTROLLER_AXIS_LEFTX;
    axisMap["LEFTY"] = SDL_CONTROLLER_AXIS_LEFTY;
    axisMap["RIGHTX"] = SDL_CONTROLLER_AXIS_RIGHTX;
    axisMap["RIGHTY"] = SDL_CONTROLLER_AXIS_RIGHTY;
    axisMap["TRIGGERLEFT"] = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
    axisMap["TRIGGERRIGHT"] = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;

    // populate reverse axis map
    for (const auto& pair : axisMap) {
        axisNameMap[pair.second] = pair.first;
    }
}
}

bool InputConfigLoader::loadFromFile(InputManager& inputManager, const std::string& filepath) {
    initializeMaps();

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "InputConfigLoader: Failed to open file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        std::string_view sv(line);
        sv = trim(sv);

        if (sv.empty() || sv[0] == '#' || sv[0] == '[') continue;

        auto equalsPos = sv.find('=');
        if (equalsPos == std::string_view::npos) {
            std::cerr << "InputConfigLoader: Syntax error on line " << lineNumber << ": Missing '='." << std::endl;
            continue;
        }

        auto keyStr = trim(sv.substr(0, equalsPos));
        auto actionName = std::string(trim(sv.substr(equalsPos + 1)));

        auto dotPos = keyStr.find('.');
        if (dotPos == std::string_view::npos) {
            std::cerr << "InputConfigLoader: Syntax error on line " << lineNumber << ": Invalid key format '" << keyStr << "'." << std::endl;
            continue;
        }

        auto typeStr = toUpper(keyStr.substr(0, dotPos));
        auto nameStr = toUpper(keyStr.substr(dotPos + 1));

        if (typeStr == "KEY") {
            if (auto it = keyMap.find(nameStr); it != keyMap.end()) {
                inputManager.mapKeyToAction(it->second, actionName);
            } else {
                std::cerr << "InputConfigLoader: Unknown key name '" << nameStr << "' on line " << lineNumber << "." << std::endl;
            }
        } else if (typeStr == "BUTTON") {
            if (auto it = buttonMap.find(nameStr); it != buttonMap.end()) {
                inputManager.mapButtonToAction(it->second, actionName);
            } else {
                std::cerr << "InputConfigLoader: Unknown button name '" << nameStr << "' on line " << lineNumber << "." << std::endl;
            }
        } else if (typeStr == "AXIS") {
            if (auto it = axisMap.find(nameStr); it != axisMap.end()) {
                inputManager.mapAxisToAction(it->second, actionName);
            } else {
                std::cerr << "InputConfigLoader: Unknown axis name '" << nameStr << "' on line " << lineNumber << "." << std::endl;
            }
        } else {
            std::cerr << "InputConfigLoader: Unknown input type '" << typeStr << "' on line " << lineNumber << "." << std::endl;
        }
    }
    std::cout << "InputConfigLoader: Successfully loaded input bindings from " << filepath << std::endl;
    return true;
}

bool InputConfigLoader::saveToFile(const InputManager& inputManager, const std::string& filepath) {
    initializeMaps();

    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "InputConfigLoader: Failed to open file for writing: " << filepath << std::endl;
        return false;
    }

    file << "# Custom input bindings, saved by the engine.\n";
    file << "# Format: <TYPE>.<SDL_NAME> = <ACTION_NAME>\n";
    file << "# TYPE can be KEY, BUTTON, or AXIS.\n";
    file << "# SDL_NAME is the name of the key/button from SDL's documentation (case-insensitive).\n\n";
    file << "[Bindings]\n";

    // Save Keyboard Bindings
    for (const auto& pair : inputManager.getKeyToActionMap()) {
        if (auto it = keyNameMap.find(pair.first); it != keyNameMap.end()) {
            file << "KEY." << it->second << " = " << pair.second << "\n";
        }
    }

    // Save Controller Button Bindings
    for (const auto& pair : inputManager.getButtonToActionMap()) {
        if (auto it = buttonNameMap.find(pair.first); it != buttonNameMap.end()) {
            file << "BUTTON." << it->second << " = " << pair.second << "\n";
        }
    }

    // Save Controller Axis Bindings
    for (const auto& pair : inputManager.getAxisToActionMap()) {
        if (auto it = axisNameMap.find(pair.first); it != axisNameMap.end()) {
            file << "AXIS." << it->second << " = " << pair.second << "\n";
        }
    }

    if (file.good()) {
        std::cout << "InputConfigLoader: Successfully saved bindings to " << filepath << std::endl;
        return true;
    } else {
        std::cerr << "InputConfigLoader: An error occurred while writing to " << filepath << std::endl;
        return false;
    }
}
