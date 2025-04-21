//
//  Input.cpp
//  game_engine
//
//  Modified on 4/13/25 to include controller support
//

#include "Input.hpp"
#include "Component.hpp"
#include <iostream>

void Input::Init() {
    keyboard_states.clear();
    just_became_down_scancodes.clear();
    just_became_up_scancodes.clear();
    mouse_button_states.clear();
    just_became_down_buttons.clear();
    just_became_up_buttons.clear();
    mouse_position = glm::vec2(0.0f, 0.0f);
    mouse_scroll_this_frame = 0.0f;

    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cout << "Controller initialization failed: " << SDL_GetError() << std::endl;
        return;
    }
    
    SDL_GameControllerEventState(SDL_ENABLE);
    
    controllers.clear();
    controller_button_states.clear();
    just_became_down_controller_buttons.clear();
    just_became_up_controller_buttons.clear();
    controller_axis_values.clear();
    
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller) {
                controllers[i] = controller;
                std::cout << "Controller connected: " << SDL_GameControllerName(controller) << std::endl;
                
                controller_button_states[i] = std::unordered_map<SDL_GameControllerButton, INPUT_STATE>();
                for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; b++) {
                    controller_button_states[i][static_cast<SDL_GameControllerButton>(b)] = INPUT_STATE_UP;
                }
                
                controller_axis_values[i] = std::unordered_map<SDL_GameControllerAxis, float>();
                for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; a++) {
                    controller_axis_values[i][static_cast<SDL_GameControllerAxis>(a)] = 0.0f;
                }
                
                just_became_down_controller_buttons[i] = std::vector<SDL_GameControllerButton>();
                just_became_up_controller_buttons[i] = std::vector<SDL_GameControllerButton>();
            }
        }
    }
}

void Input::Shutdown() {
    // Close all open controllers
    for (auto& [id, controller] : controllers) {
        if (controller) {
            SDL_GameControllerClose(controller);
        }
    }
    controllers.clear();
}

void Input::ProcessEvent(const SDL_Event& e) {
    switch (e.type) {
        // Keyboard events
        case SDL_KEYDOWN: {
            SDL_Scancode sc = e.key.keysym.scancode;
            if (keyboard_states[sc] != INPUT_STATE_DOWN &&
                keyboard_states[sc] != INPUT_STATE_JUST_BECAME_DOWN) {
                keyboard_states[sc] = INPUT_STATE_JUST_BECAME_DOWN;
                just_became_down_scancodes.push_back(sc);
            }
            break;
        }
        case SDL_KEYUP: {
            SDL_Scancode sc = e.key.keysym.scancode;
            if (keyboard_states[sc] != INPUT_STATE_UP &&
                keyboard_states[sc] != INPUT_STATE_JUST_BECAME_UP) {
                keyboard_states[sc] = INPUT_STATE_JUST_BECAME_UP;
                just_became_up_scancodes.push_back(sc);
            }
            break;
        }
        
        // Mouse events
        case SDL_MOUSEMOTION: {
            mouse_position.x = static_cast<float>(e.motion.x);
            mouse_position.y = static_cast<float>(e.motion.y);
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            int button = e.button.button;
            if (mouse_button_states[button] != INPUT_STATE_DOWN &&
                mouse_button_states[button] != INPUT_STATE_JUST_BECAME_DOWN) {
                mouse_button_states[button] = INPUT_STATE_JUST_BECAME_DOWN;
                just_became_down_buttons.push_back(button);
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            int button = e.button.button;
            if (mouse_button_states[button] != INPUT_STATE_UP &&
                mouse_button_states[button] != INPUT_STATE_JUST_BECAME_UP) {
                mouse_button_states[button] = INPUT_STATE_JUST_BECAME_UP;
                just_became_up_buttons.push_back(button);
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            mouse_scroll_this_frame = e.wheel.preciseY;
            break;
        }
        
        // Controller connection events
        case SDL_CONTROLLERDEVICEADDED: {
            int which = e.cdevice.which;
            if (!SDL_IsGameController(which)) {
                return;
            }
            
            SDL_GameController* controller = SDL_GameControllerOpen(which);
            if (controller) {
                controllers[which] = controller;
                std::cout << "Controller connected: " << SDL_GameControllerName(controller) << std::endl;
                
                // Initialize button states for this controller
                controller_button_states[which] = std::unordered_map<SDL_GameControllerButton, INPUT_STATE>();
                for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; b++) {
                    controller_button_states[which][static_cast<SDL_GameControllerButton>(b)] = INPUT_STATE_UP;
                }
                
                // Initialize axis values for this controller
                controller_axis_values[which] = std::unordered_map<SDL_GameControllerAxis, float>();
                for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; a++) {
                    controller_axis_values[which][static_cast<SDL_GameControllerAxis>(a)] = 0.0f;
                }
                
                // Initialize vectors for tracking buttons that just changed state
                just_became_down_controller_buttons[which] = std::vector<SDL_GameControllerButton>();
                just_became_up_controller_buttons[which] = std::vector<SDL_GameControllerButton>();
            }
            break;
        }
        
        case SDL_CONTROLLERDEVICEREMOVED: {
            int which = e.cdevice.which;
            auto it = controllers.find(which);
            if (it != controllers.end()) {
                SDL_GameControllerClose(it->second);
                controllers.erase(it);
                controller_button_states.erase(which);
                just_became_down_controller_buttons.erase(which);
                just_became_up_controller_buttons.erase(which);
                controller_axis_values.erase(which);
                std::cout << "Controller disconnected" << std::endl;
            }
            break;
        }
        
        // Controller button events
        case SDL_CONTROLLERBUTTONDOWN: {
            int which = e.cbutton.which;
            SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(e.cbutton.button);
            
            if (controller_button_states[which][button] != INPUT_STATE_DOWN &&
                controller_button_states[which][button] != INPUT_STATE_JUST_BECAME_DOWN) {
                controller_button_states[which][button] = INPUT_STATE_JUST_BECAME_DOWN;
                just_became_down_controller_buttons[which].push_back(button);
            }
            break;
        }
        
        case SDL_CONTROLLERBUTTONUP: {
            int which = e.cbutton.which;
            SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(e.cbutton.button);
            
            if (controller_button_states[which][button] != INPUT_STATE_UP &&
                controller_button_states[which][button] != INPUT_STATE_JUST_BECAME_UP) {
                controller_button_states[which][button] = INPUT_STATE_JUST_BECAME_UP;
                just_became_up_controller_buttons[which].push_back(button);
            }
            break;
        }
        
        // Controller axis events
        case SDL_CONTROLLERAXISMOTION: {
            int which = e.caxis.which;
            SDL_GameControllerAxis axis = static_cast<SDL_GameControllerAxis>(e.caxis.axis);
            // Convert from -32768 to 32767 range to -1.0 to 1.0 range
            float value = e.caxis.value / 32767.0f;
            
            // Apply a small deadzone to prevent stick drift
            const float DEADZONE = 0.1f;
            if (std::abs(value) < DEADZONE) {
                value = 0.0f;
            }
            
            controller_axis_values[which][axis] = value;
            break;
        }
    }
}

void Input::LateUpdate() {
    // Update keyboard states
    for (SDL_Scancode sc : just_became_down_scancodes) {
        keyboard_states[sc] = INPUT_STATE_DOWN;
    }
    for (SDL_Scancode sc : just_became_up_scancodes) {
        keyboard_states[sc] = INPUT_STATE_UP;
    }
    just_became_down_scancodes.clear();
    just_became_up_scancodes.clear();
    
    // Update mouse button states
    for (int button : just_became_down_buttons) {
        mouse_button_states[button] = INPUT_STATE_DOWN;
    }
    for (int button : just_became_up_buttons) {
        mouse_button_states[button] = INPUT_STATE_UP;
    }
    just_became_down_buttons.clear();
    just_became_up_buttons.clear();
    
    // Reset scroll delta
    mouse_scroll_this_frame = 0.0f;
    
    // Update controller button states
    for (auto& [controller_id, buttons] : just_became_down_controller_buttons) {
        for (SDL_GameControllerButton button : buttons) {
            controller_button_states[controller_id][button] = INPUT_STATE_DOWN;
        }
        buttons.clear();
    }
    
    for (auto& [controller_id, buttons] : just_became_up_controller_buttons) {
        for (SDL_GameControllerButton button : buttons) {
            controller_button_states[controller_id][button] = INPUT_STATE_UP;
        }
        buttons.clear();
    }
}

// Keyboard input methods
bool Input::GetKey(std::string keycode) {
    auto it = __keycode_to_scancode.find(keycode);
    
    if (it == __keycode_to_scancode.end()) {
        return false;
    }
    
    INPUT_STATE state = keyboard_states[it->second];
    return state == INPUT_STATE_DOWN || state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(std::string keycode) {
    auto it = __keycode_to_scancode.find(keycode);
    
    if(it == __keycode_to_scancode.end()) {
        return false;
    }
        
    return keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(std::string keycode) {
    auto it = __keycode_to_scancode.find(keycode);
    
    if (it == __keycode_to_scancode.end()) {
        return false;
    }
    
    return keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_UP;
}

// Mouse input methods
glm::vec2 Input::GetMousePosition() {
    return mouse_position;
}

bool Input::GetMouseButton(int button) {
    INPUT_STATE state = mouse_button_states[button];
    return state == INPUT_STATE_DOWN || state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonDown(int button) {
    return mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonUp(int button) {
    return mouse_button_states[button] == INPUT_STATE_JUST_BECAME_UP;
}

float Input::GetMouseScrollDelta() {
    return mouse_scroll_this_frame;
}

void Input::HideCursor() {
    SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor() {
    SDL_ShowCursor(SDL_ENABLE);
}

// Controller helper methods
SDL_GameControllerButton Input::StringToButton(const std::string& buttonName) {
    auto it = __buttonname_to_sdl_button.find(buttonName);
    if (it != __buttonname_to_sdl_button.end()) {
        return it->second;
    }
    return SDL_CONTROLLER_BUTTON_INVALID;
}

SDL_GameControllerAxis Input::StringToAxis(const std::string& axisName) {
    auto it = __axisname_to_sdl_axis.find(axisName);
    if (it != __axisname_to_sdl_axis.end()) {
        return it->second;
    }
    return SDL_CONTROLLER_AXIS_INVALID;
}

// Controller input methods
bool Input::GetButton(int controllerIndex, std::string buttonName) {
    // Check if controller exists
    if (controllers.find(controllerIndex) == controllers.end()) {
        return false;
    }
    
    SDL_GameControllerButton button = StringToButton(buttonName);
    if (button == SDL_CONTROLLER_BUTTON_INVALID) {
        return false;
    }
    
    INPUT_STATE state = controller_button_states[controllerIndex][button];
    return state == INPUT_STATE_DOWN || state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetButtonDown(int controllerIndex, std::string buttonName) {
    // Check if controller exists
    if (controllers.find(controllerIndex) == controllers.end()) {
        return false;
    }
    
    SDL_GameControllerButton button = StringToButton(buttonName);
    if (button == SDL_CONTROLLER_BUTTON_INVALID) {
        return false;
    }
    
    return controller_button_states[controllerIndex][button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetButtonUp(int controllerIndex, std::string buttonName) {
    // Check if controller exists
    if (controllers.find(controllerIndex) == controllers.end()) {
        return false;
    }
    
    SDL_GameControllerButton button = StringToButton(buttonName);
    if (button == SDL_CONTROLLER_BUTTON_INVALID) {
        return false;
    }
    
    return controller_button_states[controllerIndex][button] == INPUT_STATE_JUST_BECAME_UP;
}

float Input::GetAxis(int controllerIndex, std::string axisName) {
    // Check if controller exists
    if (controllers.find(controllerIndex) == controllers.end()) {
        return 0.0f;
    }
    
    SDL_GameControllerAxis axis = StringToAxis(axisName);
    if (axis == SDL_CONTROLLER_AXIS_INVALID) {
        return 0.0f;
    }
    
    return controller_axis_values[controllerIndex][axis];
}

bool Input::IsControllerConnected(int controllerIndex) {
    return controllers.find(controllerIndex) != controllers.end();
}

int Input::GetConnectedControllerCount() {
    return static_cast<int>(controllers.size());
}

std::string Input::GetControllerName(int controllerIndex) {
    auto it = controllers.find(controllerIndex);
    if (it != controllers.end()) {
        return SDL_GameControllerName(it->second);
    }
    return "Not Connected";
}

bool Input::SetVibration(int controllerIndex, float leftMotor, float rightMotor, int durationMs) {
    auto it = controllers.find(controllerIndex);
    if (it == controllers.end()) {
        return false;
    }
    
    // Convert from 0.0-1.0 range to 0-65535 range
    Uint16 lowFreq = static_cast<Uint16>(leftMotor * 65535.0f);
    Uint16 highFreq = static_cast<Uint16>(rightMotor * 65535.0f);
    
    return SDL_GameControllerRumble(it->second, lowFreq, highFreq, durationMs) == 0;
}

void Input::StopVibration(int controllerIndex) {
    auto it = controllers.find(controllerIndex);
    if (it != controllers.end()) {
        SDL_GameControllerRumble(it->second, 0, 0, 0);
    }
}
