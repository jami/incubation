#include "graphic.h"
#include "log.h"
#include "configuration.h"

void update_perspective(int forced) {
    if (!forced
        && graphic_manager.context.perspective_mode == graphic_manager.context._perspective_mode) {
        return;
    }

    if (graphic_manager.context.perspective_mode == PM_3D_PERSPECTIVE) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(
            45.0f,
            graphic_manager.context.aspect_ratio,
            graphic_manager.context.near_far_clipping_2d[0],
            graphic_manager.context.near_far_clipping_2d[1]
        );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    } else if (graphic_manager.context.perspective_mode == PM_2D_ORTHOGONAL) {
        glViewport(0, 0, graphic_manager.context.orthogonal_dim_2d[0], graphic_manager.context.orthogonal_dim_2d[1]);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(
            0,
            graphic_manager.context.orthogonal_dim_2d[0],
            0,
            -graphic_manager.context.orthogonal_dim_2d[1],
            -1.0f,
            10.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0, -graphic_manager.context.orthogonal_dim_2d[1], 0);
    }

    graphic_manager.context._perspective_mode = graphic_manager.context.perspective_mode;
}

void update_rendermode(int forced) {
    if (!forced
        && graphic_manager.context.render_mode == graphic_manager.context._render_mode) {
        return;
    }

    switch (graphic_manager.context.render_mode) {
        case RM_2D_SOLID:
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
            break;
        case RM_2D_TEXTURED:
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case RM_3D_SOLID:
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case RM_3D_TEXTURED:
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case RM_NONE:
            break;
        default:
            LOG("Unknown render mode");
    }

    graphic_manager.context._render_mode = graphic_manager.context.render_mode;
}

void update_settings() {
    LOGF("Set color %f %f %f %f", array_list_4(graphic_manager.context.clear_color_4f));
    glClearColor(array_list_4(graphic_manager.context.clear_color_4f));
}

void sdl_init_gl() {
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);
}

void sdl_get_resolution() {
    SDL_Rect **modus;
    
    graphic_manager.res_mode_default = 0;
    modus = SDL_ListModes(NULL, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_OPENGL);
    if (modus == NULL) {
        LOG("Can't find any display resolutions");
        exit(EXIT_FAILURE);
    }

    if (modus == (SDL_Rect**)-1) {
        graphic_manager.res_mode_count = 3;
        graphic_manager.res_mode_list = malloc(sizeof(res_mode_t) * 3);
        graphic_manager.res_mode_list[0].width  = 800;
        graphic_manager.res_mode_list[0].height = 600;
        graphic_manager.res_mode_list[0].bpp    = 32;
        graphic_manager.res_mode_list[1].width  = 1024;
        graphic_manager.res_mode_list[1].height = 768;
        graphic_manager.res_mode_list[1].bpp    = 32;
        graphic_manager.res_mode_list[2].width  = 1240;
        graphic_manager.res_mode_list[2].height = 1024;
        graphic_manager.res_mode_list[2].bpp    = 32;

        graphic_manager.res_mode_default = &graphic_manager.res_mode_list[0];
    } else {        
        int count = 0;
        for (; modus[count]; count++) {}

        if (count == 0) {
            LOG("Can't find any display resolutions");
            exit(EXIT_FAILURE);
        }

        graphic_manager.res_mode_count = count;
        graphic_manager.res_mode_list = malloc(sizeof(res_mode_t) * count);

        int i = 0;
        for (; modus[i]; i++) {
            graphic_manager.res_mode_list[i].width  = modus[i]->w;
            graphic_manager.res_mode_list[i].height = modus[i]->h;
            graphic_manager.res_mode_list[i].bpp    = 32;

            if (configuration.resolution[0] == modus[i]->w
                && configuration.resolution[1] == modus[i]->h) {
                graphic_manager.res_mode_default = &graphic_manager.res_mode_list[i];
            }
        }
    }
}

void sdl_initialize() {
    if (graphic_manager.isLoaded) {
        return;
    }
    // Start SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    // enable unicode
    SDL_EnableUNICODE(1);

    const SDL_VideoInfo *videoinfo = SDL_GetVideoInfo();
    graphic_manager.flags = SDL_OPENGL | SDL_OPENGLBLIT | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;

    if (videoinfo->hw_available) {
        graphic_manager.flags |= SDL_HWSURFACE;
    } else {
        graphic_manager.flags |= SDL_SWSURFACE;
    }

    if (videoinfo->blit_hw) {
        graphic_manager.flags |= SDL_HWACCEL;
    }

    if (graphic_manager.fullscreen) {
        graphic_manager.flags |= SDL_FULLSCREEN;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    graphic_manager.res_mode_list  = NULL;
    graphic_manager.res_mode_count = 0;
    
    sdl_get_resolution();

    graphic_manager.surface = SDL_SetVideoMode(
        graphic_manager.res_mode_default->width,
        graphic_manager.res_mode_default->height,
        graphic_manager.res_mode_default->bpp,
        graphic_manager.flags
    );

    if (graphic_manager.surface == 0) {
	LOG(SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_ShowCursor(1);
    SDL_WM_SetCaption("Incubation", "Incubation");

    sdl_init_gl();

    graphic_manager.isLoaded = 1;
}

void sdl_finalize() {
    if (!graphic_manager.isLoaded) {
        return;
    }

    // free surface
    if (graphic_manager.surface != 0) {
        SDL_FreeSurface(graphic_manager.surface);
    }

    if (graphic_manager.res_mode_list != NULL) {
        free(graphic_manager.res_mode_list);
        graphic_manager.res_mode_default = NULL;
    }
    
    SDL_Quit();
    graphic_manager.isLoaded = 0;
}

void graphic_context_init() {
    graphic_manager.context.aspect_ratio = 4.0f / 3.0f;
    array_set_2(graphic_manager.context.near_far_clipping_2d, 0, 100);
    array_set_2(graphic_manager.context.orthogonal_dim_2d, 800, 600);
    array_set_4(graphic_manager.context.clear_color_4f, 1.0f, 0.5f, 1.0f, 1.0f);

    graphic_manager.context._render_mode = -1;
    graphic_manager.context.render_mode = RM_2D_SOLID;

    graphic_manager.context._perspective_mode = -1;
    graphic_manager.context.perspective_mode = PM_2D_ORTHOGONAL;
}

void graphic_manager_context_update() {
    update_perspective(0);
    update_rendermode(0);
    update_settings();
}

void graphic_manager_context_force_update() {
    update_perspective(1);
    update_rendermode(1);
    update_settings();
}

void graphic_manager_init() {
    graphic_manager.isLoaded = 0;
    graphic_manager.surface  = 0;
    
    sdl_initialize();

    graphic_context_init();
    graphic_manager_context_force_update();
}

void graphic_manager_shutdown() {
    sdl_finalize();
}

void graphic_manager_flush() {
    SDL_GL_SwapBuffers();
}

void graphic_manager_clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

graphic_context_p graphic_manager_context() {
    return &graphic_manager.context;
}

void graphic_manager_resolution(int width, int height) {
    int count = graphic_manager.res_mode_count;

    int i = 0;
    for (; i < count; i++) {
        if (width == graphic_manager.res_mode_list[i].width
            && height == graphic_manager.res_mode_list[i].height) {

            graphic_manager.res_mode_default = &graphic_manager.res_mode_list[i];
            return;
        }
    }
}