/* 
 * File:   configuration.h
 * Author: jami
 *
 * Created on April 18, 2012, 1:17 PM
 */
#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#include "state.h"
#include "getopt.h"
#include "ressource.h"

typedef struct _configuration_manager_t {
    state_list_p state;
    char *data_path;
    int verbose;
    int resolution[2];
    archive_p data_archive;
} configuration_manager_t;

configuration_manager_t configuration;

void configuration_manager_init(int, char**);
void configuration_manager_save();
void configuration_manager_shutdown();

#endif	/* CONFIGURATION_H */

