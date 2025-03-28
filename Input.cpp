#include "Input.hpp"
#include "Component.hpp"

using namespace std;

void Input::Init() {
    keyboard_states.clear();
    just_became_down_scancodes.clear();
    just_became_up_scancodes.clear();
    mouse_button_states.clear();
    just_became_down_buttons.clear();
    just_became_up_buttons.clear();
    mouse_position = glm::vec2(0.0f, 0.0f);
    mouse_scroll_this_frame = 0.0f;
}

void Input::ProcessEvent(const SDL_Event& e) {
    switch (e.type) {
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
}

bool Input::GetKey(string keycode) {
    auto it = __keycode_to_scancode.find(keycode);
    
    if (it == __keycode_to_scancode.end()){
        return false;
    }
    
    INPUT_STATE state = keyboard_states[it->second];
    return state == INPUT_STATE_DOWN || state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(std::string keycode){
    auto it = __keycode_to_scancode.find(keycode);
    
    if(it == __keycode_to_scancode.end()){
        return false;
    }
        
    return keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_DOWN;
}


bool Input::GetKeyUp(std::string keycode){
    auto it = __keycode_to_scancode.find(keycode);
    
    if (it == __keycode_to_scancode.end()){
        return false;
    }
    
    return keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_UP;
    
    
}


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



void Input::HideCursor(){
    SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor(){
    SDL_ShowCursor(SDL_ENABLE);
}
