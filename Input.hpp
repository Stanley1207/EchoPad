#ifndef INPUT_H
#define INPUT_H

#include "SDL/SDL.h"
#include <unordered_map>
#include <vector>
#include "glm/glm.hpp"

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

class Input
{
public:
    static void Init();
    static void ProcessEvent(const SDL_Event & e);
    static void LateUpdate();
    static void Shutdown();

    static bool GetKey(std::string keycode);
    static bool GetKeyDown(std::string keycode);
    static bool GetKeyUp(std::string keycode);
    
    static glm::vec2 GetMousePosition();
    
    static bool GetMouseButton(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);
    static float GetMouseScrollDelta();
    
    static void HideCursor();
    static void ShowCursor();
    
    // Controller input
    static bool GetButton(int controllerIndex, std::string buttonName);
    static bool GetButtonDown(int controllerIndex, std::string buttonName);
    static bool GetButtonUp(int controllerIndex, std::string buttonName);
    static float GetAxis(int controllerIndex, std::string axisName);
    static bool IsControllerConnected(int controllerIndex);
    static int GetConnectedControllerCount();
    static std::string GetControllerName(int controllerIndex);
    static bool SetVibration(int controllerIndex, float leftMotor, float rightMotor, int durationMs);
    static void StopVibration(int controllerIndex);
    
    // Convenience methods for the primary controller (index 0)
    static bool GetButton(std::string buttonName) { return GetButton(0, buttonName); }
    static bool GetButtonDown(std::string buttonName) { return GetButtonDown(0, buttonName); }
    static bool GetButtonUp(std::string buttonName) { return GetButtonUp(0, buttonName); }
    static float GetAxis(std::string axisName) { return GetAxis(0, axisName); }
    static bool IsAnyControllerConnected() { return GetConnectedControllerCount() > 0; }
    

private:
    static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
    static inline std::vector<SDL_Scancode> just_became_down_scancodes;
    static inline std::vector<SDL_Scancode> just_became_up_scancodes;
    
    static inline glm::vec2 mouse_position;
    static inline std::unordered_map<int, INPUT_STATE> mouse_button_states;
    static inline std::vector<int> just_became_down_buttons;
    static inline std::vector<int> just_became_up_buttons;
    
    static inline float mouse_scroll_this_frame = 0;
    
    static inline std::unordered_map<int, SDL_GameController*> controllers;
    static inline std::unordered_map<int, std::unordered_map<SDL_GameControllerButton, INPUT_STATE>> controller_button_states;
    static inline std::unordered_map<int, std::vector<SDL_GameControllerButton>> just_became_down_controller_buttons;
    static inline std::unordered_map<int, std::vector<SDL_GameControllerButton>> just_became_up_controller_buttons;
    static inline std::unordered_map<int, std::unordered_map<SDL_GameControllerAxis, float>> controller_axis_values;
    
    // Helper function to convert string to SDL_GameControllerButton
    static SDL_GameControllerButton StringToButton(const std::string& buttonName);
    
    // Helper function to convert string to SDL_GameControllerAxis
    static SDL_GameControllerAxis StringToAxis(const std::string& axisName);
};

const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
    {"up", SDL_SCANCODE_UP},
    {"down", SDL_SCANCODE_DOWN},
    {"right", SDL_SCANCODE_RIGHT},
    {"left", SDL_SCANCODE_LEFT},
    {"escape", SDL_SCANCODE_ESCAPE},
    {"lshift", SDL_SCANCODE_LSHIFT},
    {"rshift", SDL_SCANCODE_RSHIFT},
    {"lctrl", SDL_SCANCODE_LCTRL},
    {"rctrl", SDL_SCANCODE_RCTRL},
    {"lalt", SDL_SCANCODE_LALT},
    {"ralt", SDL_SCANCODE_RALT},
    {"tab", SDL_SCANCODE_TAB},
    {"return", SDL_SCANCODE_RETURN},
    {"enter", SDL_SCANCODE_RETURN},
    {"backspace", SDL_SCANCODE_BACKSPACE},
    {"delete", SDL_SCANCODE_DELETE},
    {"insert", SDL_SCANCODE_INSERT},
    {"space", SDL_SCANCODE_SPACE},
    {"a", SDL_SCANCODE_A},
    {"b", SDL_SCANCODE_B},
    {"c", SDL_SCANCODE_C},
    {"d", SDL_SCANCODE_D},
    {"e", SDL_SCANCODE_E},
    {"f", SDL_SCANCODE_F},
    {"g", SDL_SCANCODE_G},
    {"h", SDL_SCANCODE_H},
    {"i", SDL_SCANCODE_I},
    {"j", SDL_SCANCODE_J},
    {"k", SDL_SCANCODE_K},
    {"l", SDL_SCANCODE_L},
    {"m", SDL_SCANCODE_M},
    {"n", SDL_SCANCODE_N},
    {"o", SDL_SCANCODE_O},
    {"p", SDL_SCANCODE_P},
    {"q", SDL_SCANCODE_Q},
    {"r", SDL_SCANCODE_R},
    {"s", SDL_SCANCODE_S},
    {"t", SDL_SCANCODE_T},
    {"u", SDL_SCANCODE_U},
    {"v", SDL_SCANCODE_V},
    {"w", SDL_SCANCODE_W},
    {"x", SDL_SCANCODE_X},
    {"y", SDL_SCANCODE_Y},
    {"z", SDL_SCANCODE_Z},
    {"0", SDL_SCANCODE_0},
    {"1", SDL_SCANCODE_1},
    {"2", SDL_SCANCODE_2},
    {"3", SDL_SCANCODE_3},
    {"4", SDL_SCANCODE_4},
    {"5", SDL_SCANCODE_5},
    {"6", SDL_SCANCODE_6},
    {"7", SDL_SCANCODE_7},
    {"8", SDL_SCANCODE_8},
    {"9", SDL_SCANCODE_9},
    {"/", SDL_SCANCODE_SLASH},
    {";", SDL_SCANCODE_SEMICOLON},
    {"=", SDL_SCANCODE_EQUALS},
    {"-", SDL_SCANCODE_MINUS},
    {".", SDL_SCANCODE_PERIOD},
    {",", SDL_SCANCODE_COMMA},
    {"[", SDL_SCANCODE_LEFTBRACKET},
    {"]", SDL_SCANCODE_RIGHTBRACKET},
    {"\\", SDL_SCANCODE_BACKSLASH},
    {"'", SDL_SCANCODE_APOSTROPHE},
    {"`", SDL_SCANCODE_GRAVE}
};


const std::unordered_map<std::string, SDL_GameControllerButton> __buttonname_to_sdl_button = {
    {"a", SDL_CONTROLLER_BUTTON_A},
    {"b", SDL_CONTROLLER_BUTTON_B},
    {"x", SDL_CONTROLLER_BUTTON_X},
    {"y", SDL_CONTROLLER_BUTTON_Y},
    {"back", SDL_CONTROLLER_BUTTON_BACK},
    {"guide", SDL_CONTROLLER_BUTTON_GUIDE},
    {"start", SDL_CONTROLLER_BUTTON_START},
    {"leftstick", SDL_CONTROLLER_BUTTON_LEFTSTICK},
    {"rightstick", SDL_CONTROLLER_BUTTON_RIGHTSTICK},
    {"leftshoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
    {"rightshoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
    {"dpad_up", SDL_CONTROLLER_BUTTON_DPAD_UP},
    {"dpad_down", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
    {"dpad_left", SDL_CONTROLLER_BUTTON_DPAD_LEFT},
    {"dpad_right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
};

const std::unordered_map<std::string, SDL_GameControllerAxis> __axisname_to_sdl_axis = {
    {"leftx", SDL_CONTROLLER_AXIS_LEFTX},
    {"lefty", SDL_CONTROLLER_AXIS_LEFTY},
    {"rightx", SDL_CONTROLLER_AXIS_RIGHTX},
    {"righty", SDL_CONTROLLER_AXIS_RIGHTY},
    {"triggerleft", SDL_CONTROLLER_AXIS_TRIGGERLEFT},
    {"triggerright", SDL_CONTROLLER_AXIS_TRIGGERRIGHT},
};


#endif
