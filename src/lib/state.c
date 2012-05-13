#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "state.h"
#include "event.h"

state_p state_create(char *name) {
    state_p state        = malloc(sizeof(state_t));
    state->next_state    = 0;
    state->prev_state    = 0;
    state->_constructor  = 0;
    state->_destructor   = 0;
    state->_update       = 0;
    state->_state_switch = 0;
    state->name          = strdup(name);

    return state;
}

state_destroy(state_p state) {
    if (state->name) {
        free(state->name);
    }
}

state_list_p state_list_create() {
    state_list_p list = malloc(sizeof(state_list_t));
    list->count  = 0;
    list->head   = 0;
    list->tail   = 0;
    list->active = 0;
}

void state_list_destroy(state_list_p state_list) {
    state_p cur = state_list->head;
    state_p tmp = 0;

    if (state_list->active) {
        state_list->active->_destructor();
        state_list->active = 0;
    }

    while (cur) {
        tmp = cur;
        cur = cur->next_state;
        state_destroy(tmp);
        free(tmp);
    }
}

void state_list_add(state_list_p state_list, state_p state) {
    state_p cur = state_list->head;
    state_p tmp;

    state->next_state = 0;

    if (cur == 0) {
        state_list->head = state;
        state_list->tail = state;
        state->prev_state = 0;
        state_list->count++;
        return;
    }

    while (cur) {
        tmp = cur;
        cur = cur->next_state;
    }

    tmp->next_state = state;
    state->prev_state = tmp;
    state_list->tail = state;
    state_list->count++;
}

void state_list_active(state_list_p state_list, char *name) {
    state_p match = 0, cur = state_list->head;

    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            match = cur;
            break;
        }
        cur = cur->next_state;
    }

    if (!match) {
        char msg[100] = "Can't find state ";
        strcat(msg, name);
        LOG(msg);
        return;
    }

    if (state_list->active == match) {
        LOG("Call same state again. Do nothing");
        return;
    }

    if (state_list->active) {
        state_list->active->_destructor();
        event_manager_delete_handler(EVENT_TYPE_UPDATE, state_list->active->_update);
    }

    match->_constructor();
    event_manager_add_handler(EVENT_TYPE_UPDATE, match->_update);

    state_list->active = match;
}