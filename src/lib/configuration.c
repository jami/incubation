#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "log.h"
#include "configuration.h"


void configuration_manager_init(int argc, char **argv) {
    configuration.data_path     = 0;
    configuration.state         = 0;
    configuration.verbose       = 0;
    configuration.data_archive  = 0;
    configuration.resolution[0] = 800;
    configuration.resolution[1] = 600;
    
    int option_pos = 0;
    static struct option argument_map[] = {
        {"datapath",  required_argument, 0, 'd'},
        {0, 0, 0, 0}
    };

    optind = 0;

    while (1) {
        int option_index = 0;
        option_pos = getopt_long(argc, argv, "d:", argument_map, &option_index);

        if (option_pos == -1)
            break;

        switch (option_pos) {
            case 0:
                /* If this option set a flag, do nothing else now. */
                /*if (argument_map[option_index].flag != 0)
                    break;

                printf ("option %s", argument_map[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);

                printf ("\n");*/
                break;
            case 'd':
                configuration.data_path = strdup(optarg);
                break;
            case '?':
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (!configuration.data_path) {
        LOG("Could not load data");
        exit(EXIT_FAILURE);
    }

    configuration.data_archive = load_archive_from_file(configuration.data_path);
    
    if (!configuration.data_archive) {
        LOGF("Could not load data file %s", configuration.data_path);
        exit(EXIT_FAILURE);
    }

    configuration.state = state_list_create();
}

void configuration_manager_save() {

}

void configuration_manager_shutdown() {
    if (configuration.data_archive) {
        archive_destroy(configuration.data_archive);
    }

    if (configuration.state) {
        state_list_destroy(configuration.state);
        free(configuration.state);
        configuration.state = 0;
    }

    if (configuration.data_path) {
        free(configuration.data_path);
        configuration.data_path = 0;
    }
}