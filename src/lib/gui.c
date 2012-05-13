#include "gui.h"
#include "log.h"
#include "json.h"

void gui_set_config(gui_p gui, json_node_p gui_config) {
    gui->image = image_create(gui->name);


}

gui_p gui_create(char *ressource_name) {
    ressource_p ressource = RESSOURCE_FIND(ressource_name);

    if (!ressource) {
        LOGF("Can't find image ressource %s", ressource_name);
        return 0;
    }

    json_parser_init();
    json_node_p gui_config = JSON_NODE_ROOT(json_parse_ressource(ressource));

    if (!gui_config) {
        LOGF("Can't find image config %s", ressource_name);
        return 0;
    }

    gui_p gui = (gui_p)malloc(sizeof(gui_t));
    gui->image = 0;
    gui->frame.array_count = 0;
    gui->frame.child = 0;
    gui->frame.parent = 0;
    gui->frame.state = 0;

    gui->name = ressource_name;
    gui_set_config(gui, gui_config);

    return gui;
}

void gui_destroy(gui_p gui) {
    if (gui->image) {
        image_destroy(gui->image);
    }
    
    free(gui);
}

void gui_load_from_ressource(gui_p gui, ressource_p ressource) {
    
}

gui_element_p gui_element_create() {

}

void gui_element_destroy(gui_element_p) {

}


