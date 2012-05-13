#include "image.h"
#include "ressource.h"
#include "json.h"
#include "log.h"
#include "configuration.h"
#include <SDL/SDL_image.h>

void image_load_from_ressource(image_p image, ressource_p ressource) {
    int data_size               = ressource->buffer_length;
    char* buffer                = ressource->stream;
    SDL_RWops *container        = SDL_RWFromMem(buffer, data_size);
    SDL_Surface *surface        = IMG_Load_RW(container, 1);
    SDL_Surface *format_surface = 0;

    if (!surface) {
        LOGF("Error while loading texture %s", SDL_GetError());
        return;
    }

    image->width = surface->w;
    image->height = surface->h;
    image->storagetype = GL_RGBA;
    image->minfilter = GL_NEAREST;
    image->magfilter = GL_NEAREST;
    image->wrap_s = GL_CLAMP;
    image->wrap_t = GL_CLAMP;

    SDL_PixelFormat format;
    format.palette = 0;
    format.colorkey = 0;
    format.alpha = 0;
    format.BitsPerPixel = 32;
    format.BytesPerPixel = 4;

    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        format.Rmask = 0xFF000000;
        format.Rshift = 0;
        format.Rloss = 0;
        format.Gmask = 0x00FF0000;
        format.Gshift = 8;
        format.Gloss = 0;
        format.Bmask = 0x0000FF00;
        format.Bshift = 16;
        format.Bloss = 0;
        format.Amask = 0x000000FF;
        format.Ashift = 24;
        format.Aloss = 0;
    } else {
        format.Rmask = 0x000000FF;
        format.Rshift = 24;
        format.Rloss = 0;
        format.Gmask = 0x0000FF00;
        format.Gshift = 16;
        format.Gloss = 0;
        format.Bmask = 0x00FF0000;
        format.Bshift = 8;
        format.Bloss = 0;
        format.Amask = 0xFF000000;
        format.Ashift = 0;
        format.Aloss = 0;
    }

    format_surface = SDL_ConvertSurface(surface, &format, SDL_SWSURFACE);
    //destsurf = SDL::fliphorizontal(destsurf);

    glGenTextures(1, &image->texture);
    glBindTexture(GL_TEXTURE_2D, image->texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        4,
        image->width,
        image->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        format_surface->pixels
    );

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, image->wrap_s);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, image->wrap_t);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, image->minfilter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, image->magfilter);

    if (image->mipmap) {
	gluBuild2DMipmaps(
            GL_TEXTURE_2D,
            image->storagetype,
            image->width,
            image->height,
            image->storagetype,
            GL_UNSIGNED_BYTE,
            format_surface->pixels
        );
    }

    SDL_FreeSurface(format_surface);
    SDL_FreeSurface(surface);
}

void image_set_config(image_p image, json_node_p image_config) {
    json_node_p area_list = json_node_get(image_config, ".area");
    image->area_count = 0;
    image->area_list  = 0;

    if (area_list && JSON_NODE_TYPE_VALUE_OBJECT == area_list->value_type) {
        int it, area_it;
        JSON_PROPERTY_ITERATE(area_list, it) {
            json_node_p area = JSON_VALUE_PROPERTY_P(area_list, it);
            if (area && 4 == JSON_VALUE_PROPERTY_COUNT_P(area)) {
                image->area_count++;
                image->area_list = (image_area_p*)realloc(image->area_list, sizeof(image_area_p) * image->area_count);
                image_area_p image_area = malloc(sizeof(image_area_t));
                image->area_list[image->area_count - 1] = image_area;
                image_area->name = JSON_VALUE_KEY_P(area);
                image_area->rect_f[0] = image_area->rect_f[1]
                = image_area->rect_f[2] = image_area->rect_f[3] = 0;

                for (area_it = 0; area_it < area->array_count && area_it < 4; area_it++) {
                    json_node_p area_comp = JSON_VALUE_PROPERTY_P(area, area_it);
                    double comp = JSON_VALUE_NUMBER_P(area_comp);
                    image_area->rect_f[area_it] = comp > 0.000001 ? (float)comp : 0;
                }
            }
        }
    }

    char *mipmap = json_node_get_bool(image_config, ".setting.mipmap");
    image->mipmap = mipmap && *mipmap ? 1 : 0;
}

image_p image_create(char *ressource_name) {
    ressource_p ressource = RESSOURCE_FIND(ressource_name);

    if (!ressource) {
        LOGF("Can't find image ressource %s", ressource_name);
        return 0;
    }

    json_parser_init();
    json_node_p image_config = JSON_NODE_ROOT(json_parse_ressource(ressource));

    if (!image_config) {
        LOGF("Can't find image config %s", ressource_name);
        return 0;
    }

    image_p image = (image_p)malloc(sizeof(image_t));
    image_set_config(image, image_config);

    image->name = json_node_get_string(image_config, ".texture");
    ressource_p texture = RESSOURCE_FIND(image->name);
    if (!texture) {
        LOGF("Can't load texture %s - %s", ressource->ressource_name, texture);
        image_destroy(image);
        return 0;
    }

    image_load_from_ressource(image, texture);
    return image;
}

void image_destroy(image_p image) {
    if (!image) {
        return;
    }

    if (image->area_list) {
        int i = 0;
        for (; i < image->area_count; i++) {
            free(image->area_list[i]);
        }
        free(image->area_list);
    }

    free(image);
}

void image_set(image_p image) {
    glBindTexture(GL_TEXTURE_2D, image->texture);
}
