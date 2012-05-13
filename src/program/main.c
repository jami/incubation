/* 
 * File:   main.c
 * Author: jami
 *
 * Created on April 18, 2012, 11:47 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "configuration.h"
#include "graphic.h"
#include "event.h"
#include "intro_state.h"
#include "timer.h"

void setup_game_stage() {
    state_list_add(configuration.state, create_intro_state());

    /* start the beast */
    state_list_active(configuration.state, "intro");
}

/**
 * main game loop
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv) {
    LOG("Hello message log");
    event_manager_init();
    configuration_manager_init(argc, argv);
    graphic_manager_init();

    setup_game_stage();
    char next_state[255];

    timer_setup();

    while (event_manager_poll()) {
        if (configuration.state->active
            && configuration.state->active->_state_switch(next_state)) {
            state_list_active(configuration.state, next_state);
        }

        timer_update();
    }

    configuration_manager_save();
    event_manager_shutdown();
    configuration_manager_shutdown();
    graphic_manager_shutdown();
    return (EXIT_SUCCESS);
}

