#ifndef __INTPUTS_H__
#define __INTPUTS_H__

#include "util.h"

struct InputManager
{
    Uint8 *previous_keyboard_state;
    Uint8 *keyboard_state;
    int key_length;

    Uint32 previous_mouse_state;
    Uint32 mouse_state;

    int mouse_x;
    int mouse_y;
    float lastX;
    float lastY;
};

enum MOUSE_BUTTONS
{
    MOUSE_BTN_LEFT = 0,
    MOUSE_BTN_RIGHT,
    MOUSE_BTN_MIDDLE,
    MOUSE_BTN_BACK,
    MOUSE_BTN_FORWARD
};

struct InputManager InputManager_Create();
void Input_Update(struct InputManager *in);
void InputManager_Destroy(struct InputManager in);

// KEYBOARD
bool Input_Key_Pressed(const struct InputManager in, const SDL_Scancode code);
bool Input_Key_Down(const struct InputManager in, const SDL_Scancode code);
bool Input_Key_Released(const struct InputManager in, const SDL_Scancode code);

// MOUSE
bool Input_Mouse_Button_Down(const struct InputManager in, const enum MOUSE_BUTTONS button);
bool Input_Mouse_Button_Pressed(const struct InputManager in, const enum MOUSE_BUTTONS button);
bool Input_Mouse_Button_Released(const struct InputManager in, const enum MOUSE_BUTTONS button);
void Input_Mouse_Position(const struct InputManager in, int *x, int *y);

#endif // __INTPUTS_H__