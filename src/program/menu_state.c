#include "menu_state.h"
#include <string.h>
#include "configuration.h"
#include "log.h"
#include "graphic.h"
#include "timer.h"
#include "ressource.h"
#include "gui.h"

static int switchToCredits = 0;
static int switchToGame = 0;
static long fpscounter = 1;

void menu_constructor() {
    LOG("menu constructor");
    graphic_context_p context = graphic_manager_context();
    array_set_4(context->clear_color_4f, 0.5f, 0.5f, 0.5f, 1.0f);
    context->perspective_mode = PM_2D_ORTHOGONAL;
    context->render_mode = RM_2D_SOLID;
    graphic_manager_context_force_update();
}

void menu_destructor() {
    LOG("menu destructor");
}

void menu_update() {
    graphic_manager_clear();



    graphic_manager_flush();
}

int menu_switch(char *active_state_name) {
    if (switchToCredits) {
        strcpy(active_state_name, "credits");
        return 1;
    } else if (switchToGame) {
        strcpy(active_state_name, "game");
        return 1;
    }

    return 0;
}

state_p create_menu_state() {
    state_p state = state_create("menu");

    state->_constructor  = &menu_constructor;
    state->_destructor   = &menu_destructor;
    state->_update       = &menu_update;
    state->_state_switch = &menu_switch;

    return state;
}