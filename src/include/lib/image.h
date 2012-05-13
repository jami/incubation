/* 
 * File:   image.h
 * Author: jami
 *
 * Created on April 20, 2012, 2:14 AM
 */

#ifndef IMAGE_H
#define	IMAGE_H

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

typedef struct _image_area_t {
    char *name;
    float rect_f[4];
} image_area_t, *image_area_p;

typedef struct _image_t {
    char *name;
    int width;
    int height;
    int mipmap;
    GLuint texture;
    GLuint storagetype;
    GLuint minfilter;
    GLuint magfilter;
    GLuint wrap_s;
    GLuint wrap_t;
    int area_count;
    image_area_p *area_list;
} image_t, *image_p;
/**
 * loads a image from archive
 * @param char* ressource name
 * @return image_p
 */
image_p image_create(char*);
void image_destroy(image_p);
void image_set(image_p);

#endif	/* IMAGE_H */

