#include <string.h>
#include "intro_state.h"
#include "configuration.h"
#include "log.h"
#include "graphic.h"
#include "timer.h"
#include "ressource.h"

static int switchToCredits = 0;
static int switchToGame = 0;
static long fpscounter = 1;

void intro_constructor() {
    LOG("intro constructor");
    graphic_context_p context = graphic_manager_context();
    array_set_4(context->clear_color_4f, 0.5f, 0.5f, 0.5f, 1.0f);
    context->perspective_mode = PM_2D_ORTHOGONAL;
    context->render_mode = RM_2D_SOLID;
    graphic_manager_context_force_update();

    //intro_struct.background_ressource = ressource_from_data("intro/logo.png");
}

void intro_destructor() {
    LOG("intro destructor");

    //ressource_destroy(intro_struct.background_ressource);
}

void intro_update() {
    graphic_manager_clear();

    glBegin(GL_QUADS);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
        glColor3f(1, 1, 0); glVertex3f(100, 0, 0);
        glColor3f(1, 0, 1); glVertex3f(100, 100, 0);
        glColor3f(1, 1, 1); glVertex3f(0, 100, 0);
    glEnd();

    if (fpscounter++ % 1000 == 0) {
        LOGF("Frames per second %f", timer.fps);
        fpscounter = 1;
    }

    graphic_manager_flush();
}

int intro_switch(char *active_state_name) {
    if (switchToCredits) {
        strcpy(active_state_name, "credits");
        return 1;
    } else if (switchToGame) {
        strcpy(active_state_name, "game");
        return 1;
    }

    return 0;   
}

state_p create_intro_state() {
    state_p state = state_create("intro");

    state->_constructor  = &intro_constructor;
    state->_destructor   = &intro_destructor;
    state->_update       = &intro_update;
    state->_state_switch = &intro_switch;

    return state;
}