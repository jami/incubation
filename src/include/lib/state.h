/* 
 * File:   state.h
 * Author: jami
 *
 * Created on April 18, 2012, 1:25 PM
 */

#ifndef STATE_H
#define	STATE_H

typedef struct _state_t {
    char *name;
    struct _state_t *next_state;
    struct _state_t *prev_state;

    void (*_destructor)();
    void (*_constructor)();
    void (*_update)();
    int (*_state_switch)(char*);
} state_t, *state_p;

typedef struct _state_list_t {
    state_p active;
    state_p head;
    state_p tail;
    int count;
} state_list_t, *state_list_p;

state_p state_create(char*);

state_list_p state_list_create();
void state_list_destroy(state_list_p);

void state_list_add(state_list_p, state_p);
void state_list_active(state_list_p, char*);

#endif	/* STATE_H */

