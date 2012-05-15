#include "gui.h"
#include "log.h"
#include "json.h"
#include "configuration.h"



void gui_element_set_button(gui_element_p element, json_node_p config) {

}

void gui_element_set_frame(gui_element_p element, json_node_p config) {

}

void gui_load_from_ressource(gui_p gui, ressource_p ressource) {

}

void gui_element_destroy(gui_element_p element) {

}

gui_element_p gui_element_create(gui_element_p parent, json_node_p config) {

    char *name = json_node_get_string(config, ".name");
    char *type = json_node_get_string(config, ".type");

    if (0 == name || 0 == type) {        
        return 0;
    }

    gui_element_p element = (gui_element_p)malloc(sizeof(gui_element_t));
    element->name = name;

    if (strcmp("frame", type) == 0) {
        element = (gui_element_p)malloc(sizeof(gui_element_t));
        gui_element_set_frame(element, config);
    } else if (strcmp("button", type) == 0) {
        element = (gui_element_p)malloc(sizeof(gui_element_t));
        gui_element_set_frame(element, config);
    } else {
        LOGF("Invalid gui element type %s", type);
        return 0;
    }
}

void gui_set_config(gui_p gui, json_node_p gui_config) {
    gui->image = image_create(gui->name);

    json_node_p frame = json_node_get(gui_config, ".frame");

    if (frame && JSON_VALUE_TYPE_P(frame) == JSON_NODE_TYPE_ARRAY) {
        if (0 == gui_element_create(&gui->frame, frame)) {
            LOGF("invalid gui frame configuration %s", gui->name);
            return;
        }
    }


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

    if (gui->frame.array_count > 0) {
        int i = 0;
        for (; i < gui->frame.array_count; i++) {
            gui_element_destroy(gui->frame.child[i]);
        }
        free(gui->frame.child);
    }

    free(gui);
}



