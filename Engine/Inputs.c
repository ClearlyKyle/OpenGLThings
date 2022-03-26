#include "Inputs.h"

struct InputManager InputManager_Create()
{
    struct InputManager in;

    const Uint8 *keyboard_state = SDL_GetKeyboardState(&in.key_length);

    in.keyboard_state = (Uint8 *)malloc(sizeof(Uint8) * in.key_length);
    in.previous_keyboard_state = (Uint8 *)malloc(sizeof(Uint8) * in.key_length);

    // Setting mPrevKeyboardState to be the same length as keyboard_state
    // Uint8 *previous_keyboard_state = (Uint8 *)calloc(in.key_length, 1);
    // in.previous_keyboard_state = previous_keyboard_state;
    (void)memcpy(in.keyboard_state, keyboard_state, in.key_length);

    // Copying the contents of keyboard_state into previous_keyboard_state so that it contains data on the first frame
    (void)memcpy(in.previous_keyboard_state, in.keyboard_state, in.key_length);

    // Set initial mouse values
    in.mouse_state = SDL_GetMouseState(&in.mouse_x, &in.mouse_y);

    return in;
}

void Input_Update(struct InputManager *in)
{
    const Uint8 *current_key_state = SDL_GetKeyboardState(NULL);

    (void)memcpy(in->previous_keyboard_state, in->keyboard_state, in->key_length);
    (void)memcpy(in->keyboard_state, current_key_state, in->key_length);

    SDL_PumpEvents();
    in->previous_mouse_state = in->mouse_state;
    in->mouse_state = SDL_GetMouseState(&in->mouse_x, &in->mouse_y);
}

bool Input_Key_Down(const struct InputManager in, const SDL_Scancode code)
{ // Being held down
    return (in.keyboard_state[code] != 0);
}

bool Input_Key_Pressed(const struct InputManager in, const SDL_Scancode code)
{ // returning true of the key was not pressed in the previous keyboard state but is pressed in the current one
    return (in.previous_keyboard_state[code] == 0 && in.keyboard_state[code] != 0);
}

bool Input_Key_Released(const struct InputManager in, const SDL_Scancode code)
{ // returning true of the key was pressed in the previous keyboard state but is not pressed in the current one
    return (in.previous_keyboard_state[code] != 0 && in.keyboard_state[code] == 0);
}

static Uint32 _Get_Mouse_Button_Mask(const enum MOUSE_BUTTONS button)
{
    Uint32 mask = 0;

    switch (button)
    {
    case MOUSE_BTN_LEFT:
        mask = SDL_BUTTON_LMASK;
        break;
    case MOUSE_BTN_RIGHT:
        mask = SDL_BUTTON_RMASK;
        break;
    case MOUSE_BTN_MIDDLE:
        mask = SDL_BUTTON_MMASK;
        break;
    case MOUSE_BTN_BACK:
        mask = SDL_BUTTON_X1MASK;
        break;
    case MOUSE_BTN_FORWARD:
        mask = SDL_BUTTON_X2MASK;
        break;
    }
    return mask;
}

bool Input_Mouse_Button_Down(const struct InputManager in, const enum MOUSE_BUTTONS button)
{
    const Uint32 mask = _Get_Mouse_Button_Mask(button);
    return ((in.mouse_state & mask) != 0);
}

bool Input_Mouse_Button_Pressed(const struct InputManager in, const enum MOUSE_BUTTONS button)
{
    // return true if the mask does not exist in the previous mouse state, but exists in the current one
    const Uint32 mask = _Get_Mouse_Button_Mask(button);
    return ((in.previous_mouse_state & mask) == 0) && ((in.mouse_state & mask) != 0);
}

bool Input_Mouse_Button_Released(const struct InputManager in, const enum MOUSE_BUTTONS button)
{
    // return true if the mask exists in the previous mouse state, but does not exist in the current one
    const Uint32 mask = _Get_Mouse_Button_Mask(button);
    return ((in.previous_mouse_state & mask) != 0) && ((in.mouse_state & mask) == 0);
}

void Input_Mouse_Position(const struct InputManager in, int *x, int *y)
{
    *x = in.mouse_x;
    *y = in.mouse_y;
}

void InputManager_Destroy(struct InputManager in)
{
    free(in.keyboard_state);
    free(in.previous_keyboard_state);

    in.keyboard_state = NULL;
    in.previous_keyboard_state = NULL;
}