#ifndef __SLOT_H
#define __SLOT_H

#include <SDL/SDL_mutex.h>
#include "patterns/pattern.h"
#include "core/parameter.h"
#include "core/time.h"
#include "util/color.h"
#include "hits/hit.h"

struct slot;
struct hit;


typedef struct slot{
    union {
        struct pattern * pattern;
        struct hit * hit;
    };
    void* state;
    param_state_t   alpha;
    param_state_t * param_states;
    SDL_Surface *surface;
} slot_t;

extern int n_slots;
extern int n_hit_slots;

extern slot_t slots[];
extern slot_t hit_slots[];

color_t render_composite(float x, float y);
extern SDL_mutex* patterns_updating;

#endif
