#include <stdio.h>
#include <stdlib.h>
#include "log.h"

void log_empty_ctor(char *filename) {
}

void log_empty_dtor() {
}

void log_to_std(char *filename, int line, char *message) {
    printf("[Log] %s:%d %s\n", filename, line, message);
}

void log_to_std_format(char *filename, int line, char* format, ...) {
    char dest[1024 * 16];
    va_list argptr;
    va_start(argptr, format);
    vsprintf(dest, format, argptr);
    va_end(argptr);

    log_to_std(filename, line, dest);
}

void log_file_ctor(char *filename) {
    if (log_manager.fp != NULL) {
        fclose(log_manager.fp);
    }

    log_manager.fp = fopen(filename, "a");
    if (log_manager.fp == NULL) {
        fprintf(stderr, "Can't open logfile '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
}

void log_file_dtor() {
    if (log_manager.fp != NULL) {
        fclose(log_manager.fp);
        log_manager.fp = NULL;
    }
}

void log_to_fp(char *filename, int line, char *message) {
    if (log_manager.fp != NULL) {
        fprintf(log_manager.fp, "[Log] %s:%d %s\n", filename, line, message);
    }
}

void log_to_fp_format(char *filename, int line, char* format, ...) {
    char dest[1024 * 16];
    va_list argptr;
    va_start(argptr, format);
    vsprintf(dest, format, argptr);
    va_end(argptr);

    log_to_fp(filename, line, dest);
}

