/* 
 * File:   event_manager.h
 * Author: jami
 *
 * Created on April 18, 2012, 3:36 PM
 */
#ifndef EVENT_MANAGER_H
#define	EVENT_MANAGER_H

/* observer types */
#define EVENT_TYPE_SYSTEM 0
#define EVENT_TYPE_GRAPHIC 1
#define EVENT_TYPE_STATE 2
#define EVENT_TYPE_INPUT 3
#define EVENT_TYPE_UPDATE 4
/* observer count */
#define EVENT_TYPE_COUNT 5
/* event subsystem codes */
#define EVENT_TYPE_SYSTEM_QUIT 0
#define EVENT_TYPE_SYSTEM_TICK 1
/* */
#define EVENT_TYPE_INPUT_MOUSE_MOTION 0
#define EVENT_TYPE_INPUT_MOUSE_BUTTON_DOWN 1
#define EVENT_TYPE_INPUT_MOUSE_BUTTON_UP 2
/* */
#define EVENT_TYPE_INPUT_KEY_DOWN 10
#define EVENT_TYPE_INPUT_KEY_UP 11


#include "SDL/SDL.h"

typedef struct _event_t {
    int code;
    void *data;
} event_t;

typedef event_t* event_p;

typedef void (*event_handler)(event_p);

typedef struct _event_observer_t {
    int type;
    int num_handler;
    event_handler *event_handler_list;
} event_observer_t;

typedef struct _event_manager_t {
    SDL_Event event;
    event_observer_t event_observer[EVENT_TYPE_COUNT];
} event_manager_t;

static event_manager_t event_manager;

void event_manager_init();
void event_manager_add_handler(int, event_handler);
void event_manager_delete_handler(int, event_handler);
void event_manager_trigger_handler(int, event_t*);
void event_manager_shutdown();

int event_manager_poll();

#endif	/* EVENT_MANAGER_H */

