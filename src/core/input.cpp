#include "core/input.h"

const char *button_table[] = {
    "INSERT", "PAUSE", "SCROLL", "MENU", "MOUSE_4", "MOUSE_5", "OTHER",
};

const i32 button_table_len  = sizeof(button_table) / sizeof(char *);


const char *translate_button_type(ButtonType button_type) {
    switch (button_type) {
        case INSERT:  return "INSERT";
        case PAUSE:   return "PAUSE";
        case SCROLL:  return "SCROLL";
        case MENU:    return "MENU";
        case MOUSE_4: return "MOUSE_4";
        case MOUSE_5: return "MOUSE_5";
        case OTHER:   return "OTHER";
    }

    assert(false && "Unreachable");
}

const char *translate_button_state(ButtonState button_state) {
    switch (button_state) {
        case BUTTON_PRESS:   return "PRESS";
        case BUTTON_RELEASE: return "RELEASE";
    }

    assert(false && "Unreachable");
}
