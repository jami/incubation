/* 
 * File:   log.h
 * Author: jami
 *
 * Created on April 18, 2012, 12:27 PM
 */

#ifndef LOG_H
#define	LOG_H

#include <stdio.h>
#include <stdarg.h>

struct _log_manager_t {
    FILE *fp;
    void (*log)(char*, int, char*);
    void (*log_format)(char*, int, char*, ...);
    void (*_ctor)();
    void (*_dtor)();
};

void log_empty_ctor(char*);
void log_empty_dtor();
void log_to_std(char*, int, char*);
void log_to_std_format(char*, int, char*, ...);
void log_file_ctor(char*);
void log_file_dtor();
void log_to_fp(char*, int, char*);
void log_to_fp_format(char*, int, char*, ...);

static struct _log_manager_t log_manager = {0, &log_to_std, &log_to_std_format, &log_empty_ctor, &log_empty_dtor};

#define LOG(message) \
{\
    log_manager.log(__FILE__,__LINE__,message);\
};

#define LOGF(message, ...) \
{\
    log_manager.log_format(__FILE__,__LINE__,message, ##__VA_ARGS__);\
};

#endif	/* LOG_H */

