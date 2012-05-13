/* 
 * File:   sprite.h
 * Author: jami
 *
 * Created on May 7, 2012, 11:47 PM
 */

#ifndef SPRITE_H
#define	SPRITE_H

typedef struct _sprite_t {
    float pos[2];
    float width;
    float height;
    float angle;
} sprite_t, *sprite_p;

/**
 * loads a sprite from archive
 * @param char* ressource name
 * @return image_p
 */
sprite_p sprite_create(char*);
/**
 * implicite destroy
 * @param sprite_p
 */
void sprite_destroy(sprite_p);


#endif	/* SPRITE_H */

