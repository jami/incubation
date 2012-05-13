#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <zlib.h>
#include <string.h>
#include "ressource.h"
#include "log.h"
#include "configuration.h"

#define ZLIB_BUFFER_SIZE 1000
#define TAR_HEADER_OFFSET 512

void get_file_entries_from_tar(archive_p archive) {
    if (archive->buffer_size <= 512) {
        LOGF("Archive buffer (%d bytes) is to small for tar header %s",
            archive->buffer_size, archive->archive_path);
        return;
    }

    int last_pos = 0;
    while ((last_pos + TAR_HEADER_OFFSET) < archive->buffer_size)  {
        tar_raw_t header = {0};
        memcpy(&header, (char*)(&archive->buffer[last_pos]), TAR_HEADER_OFFSET);

        if (header.filename[0] == 0) {
            break;
        }

        last_pos += TAR_HEADER_OFFSET;
        /* calc filesize from octal to dec */
        int i = 0, j = strlen(header.filesize) - 1;
        int filesize = 0;
        for (; i < (strlen(header.filesize) - 1); i++, j--) {
            int d  = header.filesize[j] - '0';
            int b  = (int)pow(8, i);
            filesize += d * b;
        }

        /* detect broken range */
        if ((last_pos + filesize) > archive->buffer_size) {
             LOGF("Archive buffer overflow. Header file size is bigger than file %s",
                 archive->archive_path);
        }
        /* skip irregular files */
        if (header.type >= '1' && header.type <= '6' ) {
            continue;
        }

        archive->index_size++;
        archive->index   = (ressource_p)realloc(archive->index,
            sizeof(ressource_t) * archive->index_size);
        ressource_p entry     = &archive->index[archive->index_size - 1];
        entry->buffer_length  = filesize;
        entry->buffer_pos     = last_pos;
        entry->ressource_name = strdup(header.filename);
        entry->stream         = &archive->buffer[last_pos];
        /*  % 512 == 0 alignment */
        last_pos += filesize > 0 ?
            (((filesize/TAR_HEADER_OFFSET) + 1) * TAR_HEADER_OFFSET) : 0;
    }
}

archive_p load_archive_from_file(char *archive_filename) {
    gzFile file = gzopen(archive_filename, "rb");
    unsigned char zbuffer[ZLIB_BUFFER_SIZE];
    char *buffer = 0;
    int buffer_size = 0;
    int bytes_read = 0, err = 0, last_pos = 0;

    if (!file) {
        LOGF("Can't open archive file %s", archive_filename);
        return 0;
    }

    while (1) {
        bytes_read = gzread(file, zbuffer, ZLIB_BUFFER_SIZE - 1);
        buffer = (char*)realloc(buffer, buffer_size + bytes_read);
        memcpy(&buffer[last_pos], zbuffer, bytes_read);
        buffer_size += bytes_read;

        if (bytes_read < (ZLIB_BUFFER_SIZE - 1)) {
            if (gzeof(file)) {
                break;
            } else {
                const char *error_string;
                error_string = gzerror(file, &err);
                if (err) {
                    LOGF("Error while decompressing archive: %s", error_string);
                    free(buffer);
                    gzclose(file);
                    return 0;
                }
            }
        } 

        last_pos = buffer_size;
    }

    gzclose(file);
    
    archive_p archive        = (archive_p)malloc(sizeof(archive_t));
    archive->archive_path    = strdup(archive_filename);
    archive->buffer          = buffer;
    archive->buffer_size     = buffer_size;
    archive->index           = 0;
    archive->index_size      = 0;

    get_file_entries_from_tar(archive);
    return archive;
}

ressource_p ressource_from_archive(archive_p archive, char *ressource_name) {
    int i = 0, files = archive->index_size;
    for (; i < files; i++) {
        if (0 == strncmp(ressource_name, archive->index[i].ressource_name, strlen(ressource_name))) {
            return &archive->index[i];
        }
    }

    return 0;
}

void archive_destroy(archive_p archive) {
    if (archive->buffer_size) {
        free(archive->buffer);
    }

    if (archive->archive_path) {
        free(archive->archive_path);
    }

    if (archive->index_size) {
        int i = 0;
        for (; i < archive->index_size; i++) {
            if (archive->index[i].ressource_name) {
                free(archive->index[i].ressource_name);
            }
        }
        free(archive->index);
    }

    free(archive);
}