#include "state.h"
#include "log.h"
#include "graphic.h"

#define GAME_STATE_START 1
static int game_state = 0;

void load_game_map() {
    
}

void render_game_map() {
    
}

void render_game_enemy() {
    
}

void render_game_towers() {
    
}

void render_game_hud() {
    
}

void game_constructor() {
    LOG("game constructor");
    graphic_context_p context = graphic_manager_context();
    array_set_4(context->clear_color_4f, 1.0f, 0.5f, 0.5f, 1.0f);
    context->perspective_mode = PM_2D_ORTHOGONAL;
    context->render_mode = RM_2D_SOLID;
    graphic_manager_context_force_update();
}

void game_destructor() {
    LOG("game destructor");

    //ressource_destroy(intro_struct.background_ressource);
}

void game_update() {
    graphic_manager_clear();

    render_game_map();
    render_game_enemy();
    render_game_towers();
    render_game_hud();

    graphic_manager_flush();
}

int game_switch(char *active_state_name) {
    switch (game_state) {
        case GAME_STATE_START:
            load_game_map();
            break;
        default:
            return 0;
    }

    return 0;   
}

state_p create_game_state() {
    state_p state = state_create("game");

    state->_constructor  = &game_constructor;
    state->_destructor   = &game_destructor;
    state->_update       = &game_update;
    state->_state_switch = &game_switch;

    return state;
}
