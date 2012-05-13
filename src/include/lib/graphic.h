/* 
 * File:   graphic.h
 * Author: jami
 *
 * Created on April 18, 2012, 2:11 PM
 */

#ifndef GRAPHIC_H
#define	GRAPHIC_H

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#define RM_NONE 0
#define RM_2D_SOLID 1
#define RM_2D_TEXTURED 2
#define RM_3D_SOLID 3
#define RM_3D_TEXTURED 4

#define PM_3D_PERSPECTIVE 1
#define PM_2D_ORTHOGONAL 2

#define array_set_4(x, a, b, c, d) x[0]=a;x[1]=b;x[2]=c;x[3]=d
#define array_list_4(x) x[0],x[1],x[2],x[3]
#define array_set_2(x, a, b) x[0]=a;x[1]=b

typedef struct _graphic_context_t {
    int render_mode;
    int _render_mode;
    int perspective_mode;
    int _perspective_mode;

    float aspect_ratio;
    float clear_color_4f[4];
    int orthogonal_dim_2d[2];
    int near_far_clipping_2d[2];
} graphic_context_t, *graphic_context_p;

typedef struct _res_mode_t {
    int width;
    int height;
    int bpp;
} res_mode_t;

typedef struct _graphic_manager_t {
    int isLoaded;
    int fullscreen;
    int flags;
    SDL_Surface *surface;
    int res_mode_count;
    res_mode_t *res_mode_list;
    res_mode_t *res_mode_default;
    graphic_context_t context;
} graphic_manager_t;

graphic_manager_t graphic_manager;

void graphic_manager_init();
void graphic_manager_shutdown();
void graphic_manager_clear();
void graphic_manager_flush();
graphic_context_p graphic_manager_context();
void graphic_manager_context_force_update();
void graphic_manager_context_update();

#endif	/* GRAPHIC_H */

