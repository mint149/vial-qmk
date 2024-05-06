#include "apidef.h"
#include "keyboard.h"

bool is_bmp_keyboard_master(void) {
    return BMPAPI->app.get_config()->mode == SPLIT_MASTER;
}

bool is_bmp_keyboard_left(void) {
    return BMPAPI->app.get_config()->matrix.is_left_hand;
}
