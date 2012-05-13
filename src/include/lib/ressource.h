/* 
 * File:   ressource.h
 * Author: jami
 *
 * Created on April 20, 2012, 2:20 AM
 */

#ifndef RESSOURCE_H
#define	RESSOURCE_H

typedef struct _ressource_t {
    char *ressource_name;
    long buffer_pos;
    long buffer_length;
    char *stream;
} ressource_t, *ressource_p;

typedef struct _tar_raw_t {
    char filename[100];
    char filemode[8];
    char userid[8];
    char groupid[8];
    char filesize[12];
    char lastmod[12];
    char checksum[8];
    char type;
    char link_filename[100];
    char brand[8];
    char ownername[32];
    char groupname[32];
    char device[16];
    char prefix[155];
    char _padding[12];
} tar_raw_t, *tar_raw_p;

typedef struct _archive_t {
    char *buffer;
    long buffer_size;
    char *archive_path;
    ressource_p index;
    int index_size;
} archive_t, *archive_p;

archive_p load_archive_from_file(char*);
void archive_destroy(archive_p);

ressource_p ressource_from_archive(archive_p, char*);
void ressource_destroy(ressource_p);

#define RESSOURCE_FIND(x) ressource_from_archive(configuration.data_archive, x)

#endif	/* RESSOURCE_H */

