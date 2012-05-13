#include <stdlib.h>

#include "event.h"

void event_observer_dtor(event_observer_t *observer) {
    if (observer->event_handler_list) {
        free(observer->event_handler_list);
    }
}

void event_manager_init() {
    int i = 0;
    for (; i < EVENT_TYPE_COUNT; i++) {
        event_manager.event_observer[i].type = i;
        event_manager.event_observer[i].num_handler = 0;
        event_manager.event_observer[i].event_handler_list = 0;
    }
}

void event_manager_add_handler(int event_type, event_handler handler) {
    event_observer_t *observer = &event_manager.event_observer[event_type];
    
    observer->event_handler_list = (event_handler*)realloc(
        observer->event_handler_list,
        sizeof(event_handler) * (observer->num_handler + 1)
    );

    observer->event_handler_list[observer->num_handler] = handler;
    observer->num_handler++;
}

void event_manager_delete_handler(int event_type, event_handler handler) {
    event_observer_t *observer = &event_manager.event_observer[event_type];
    if (observer->num_handler == 0) {
        return;
    }

    event_handler *new_handler_list = 0;
    int new_handler_index = 0;
    int i = 0;

    for (; i < observer->num_handler; i++) {
        if (observer->event_handler_list[i] != handler) {
            new_handler_index++;
            new_handler_list = realloc(new_handler_list, sizeof(event_handler) * new_handler_index);
            new_handler_list[new_handler_index - 1] = observer->event_handler_list[i];
        }
    }

    free(observer->event_handler_list);

    observer->event_handler_list = new_handler_list;
    observer->num_handler = new_handler_index;
}

void event_manager_trigger_handler(int event_type, event_t *event) {
    event_observer_t *observer = &event_manager.event_observer[event_type];

    int i = 0;
    for (; i < observer->num_handler; i++) {
        event_handler handler = observer->event_handler_list[i];
        handler(event);
    }
}

void event_manager_shutdown() {
    int i = 0;
    for (; i < EVENT_TYPE_COUNT; i++) {
        event_observer_dtor(&event_manager.event_observer[i]);
    }
}

int event_manager_poll() {
    int run = 1;
    static event_t tick_event = {EVENT_TYPE_SYSTEM_TICK, NULL};
    
    while (SDL_PollEvent(&event_manager.event)) {
        switch (event_manager.event.type) {
            case SDL_MOUSEMOTION:
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
                break;
            case SDL_VIDEORESIZE:
                break;
            case SDL_QUIT:
                run = 0;
                break;
        }
    }

    event_manager_trigger_handler(EVENT_TYPE_UPDATE, &tick_event);

    return run;
}
