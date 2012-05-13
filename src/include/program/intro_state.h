/* 
 * File:   intro_state.h
 * Author: jami
 *
 * Created on April 19, 2012, 7:02 PM
 */

#ifndef INTRO_STATE_H
#define	INTRO_STATE_H

#include "state.h"
#include "ressource.h"
#include "image.h"

typedef struct _intro_struct_t {
    image_t background;
} intro_struct_t;

static intro_struct_t intro_struct;

state_p create_intro_state();

#endif	/* INTRO_STATE_H */

