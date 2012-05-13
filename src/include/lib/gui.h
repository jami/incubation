/* 
 * File:   gui.h
 * Author: jami
 *
 * Created on May 12, 2012, 11:55 PM
 */
#ifndef GUI_H
#define	GUI_H

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "event.h"
#include "ressource.h"
#include "image.h"

#define GUI_STATE_HIDDEN 1
#define GUI_STATE_DISABLED 2
#define GUI_STATE_CLICKED 3
#define GUI_STATE_KEY_PRESSED 4

typedef struct _gui_element_t {
    char *name;
    int state;
    int array_count;
    struct _gui_element_t **child;
    struct _gui_element_t *parent;
    void (*update)();
} gui_element_t, *gui_element_p;

typedef struct _gui_t {
    char *name;
    image_p image;
    gui_element_t frame;
    void (*input_event_handler)(event_p);
    void (*update_event_handler)(event_p);
} gui_t, *gui_p;

gui_p gui_create(char*);
void gui_destroy(gui_p);
gui_element_p gui_element_create();
void gui_element_destroy(gui_element_p);


#endif	/* GUI_H */

