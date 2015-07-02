#include <math.h>

#include "patterns/pattern.h"
#include "core/slot.h"
#include "util/color.h"
#include "util/math.h"

// --------- Pattern: Bubble -----------

static const char name[] = "Bubble";

typedef struct
{
    color_t color;
    float r;
    float rho;
    float cx;
    float cy;
} state_t;

enum param_names {
    R,
    RHO,
    COLOR,
    CX,
    CY,

    N_PARAMS
};

static const parameter_t params[N_PARAMS] = {
    [COLOR] = {
        .name = "Color",
        .default_val = 0.5f,
    },
    [R] = {
        .name = "r",
        .default_val = 0.5f,
        .val_to_str = float_to_string,
    },
    [RHO] = {
        .name = "\\rho",
        .default_val = 0.5f,
        .val_to_str = float_to_string,
    },
    [CX] = {
        .name = "cx",
        .default_val = 0.5f,
        .val_to_str = float_to_string,
    },
    [CY] = {
        .name = "cy",
        .default_val = 0.5f,
        .val_to_str = float_to_string,
    },
};
static void init(state_t* state)
{
    state->color = (color_t) {0.f, 0.f, 0.f, 0.f};
    state->r = 0.f;
    state->cx = 0.f;
    state->cy = 0.f;
}
static void update(slot_t* slot, long t)
{
    state_t* state = (state_t*)slot->state;
    struct colormap * cm = slot->colormap ? slot->colormap : cm_global;
    state->color = colormap_color(cm, param_state_get(&slot->param_states[COLOR]));
    state->r = param_state_get(&slot->param_states[R]);
    state->rho = param_state_get(&slot->param_states[RHO]) * 1.3f + 0.3f;
    state->cx = param_state_get(&slot->param_states[CX]) * 2 - 1.f;
    state->cy = param_state_get(&slot->param_states[CY]) * 2 - 1.f;
}
static void command(slot_t* slot, pat_command_t cmd)
{
    switch(cmd.index)
    {
        case 0:
            param_state_setq(&slot->param_states[CX], cmd.value);
            break;
        case 1:
            param_state_setq(&slot->param_states[CY], cmd.value);
            break;
    }
}
static inline color_t render(const state_t* restrict state, float x, float y){
    float d;
    color_t result = state->color;
    d = hypotf(state->cx-x,state->cy-y)/state->r;
    if(d < 1.f)   result.a = powf(1.f - powf(d, state->rho), 1.f / state->rho);
    else          result.a = 0.f;
    return result;
}
MAKE_PATTERN_RENDER_IMG_FN
pattern_t pat_bubble = MAKE_PATTERN;
