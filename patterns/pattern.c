#include <stdlib.h>
#include <math.h>

#include "util/math.h"
#include "core/slot.h"
#include "core/err.h"
#include "patterns/pattern.h"
#include "util/color.h"
#include "util/siggen.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define N_PATTERNS 3
pattern_t* patterns[N_PATTERNS] = {&pat_full, &pat_wave, &pat_bubble};
int n_patterns = N_PATTERNS;

// --------- Pattern: Full -----------

enum pat_full_param_names {
    FULL_COLOR,
    FULL_VALUE,

    N_FULL_PARAMS
};

parameter_t pat_full_params[] = {
    [FULL_COLOR] = {
        .name = "Color",
        .default_val = 0.5,
    },
    [FULL_VALUE] = {
        .name = "Value",
        .default_val = 1.0,
        .val_to_str = float_to_string,
    },
};

pat_state_pt pat_full_init()
{
    color_t * color = malloc(sizeof(color_t));
    *color = (color_t) {.r = 0., .g = 0., .b = 0., .a = 0.};
    return color;
}

void pat_full_del(pat_state_pt state)
{
    free(state);
}

void pat_full_update(slot_t* slot, long t)
{
    PARAM_UNUSED t;
    color_t* color = (color_t*)slot->state;
    *color = param_to_color(param_state_get(&slot->param_states[FULL_COLOR]));
    float v = param_state_get(&slot->param_states[FULL_VALUE]);
    color->r *= v;
    color->g *= v;
    color->b *= v;
}

void pat_full_prevclick(slot_t * slot, float x, float y){
  PARAM_UNUSED slot;
  PARAM_UNUSED x;
  PARAM_UNUSED y;

}

color_t pat_full_pixel(slot_t* slot, float x, float y)
{
    PARAM_UNUSED x;
    PARAM_UNUSED y;
    return *(color_t*)slot->state;
}

pattern_t pat_full = {
    .render = &pat_full_pixel,
    .init = &pat_full_init,
    .del = &pat_full_del,
    .update = &pat_full_update,
    .prevclick = &pat_full_prevclick,
    .n_params = N_FULL_PARAMS,
    .parameters = pat_full_params,
    .name = "Full",
};

// --------- Pattern: Wave -----------

typedef struct
{
    color_t color;
    float phase;
    enum osc_type type;
    mbeat_t last_t;
    float kx;
    float ky;
} pat_wave_state_t;

enum pat_wave_param_names {
    WAVE_COLOR,
    WAVE_TYPE,
    WAVE_OMEGA,
    WAVE_K_MAG,
    WAVE_K_ANGLE,

    N_WAVE_PARAMS
};

parameter_t pat_wave_params[N_WAVE_PARAMS] = {
    [WAVE_COLOR] = {
        .name = "Color",
        .default_val = 0.5,
    },
    [WAVE_TYPE] = {
        .name = "Wave Type",
        .default_val = 0.0,
    },
    [WAVE_OMEGA] = {
        .name = "\\omega",
        .default_val = 0.5,
        .val_to_str = power_quantize_parameter_label,
    },
    [WAVE_K_MAG] = {
        .name = "|k|",
        .default_val = 0.5,
        .val_to_str = float_to_string,
    },
    [WAVE_K_ANGLE] = {
        .name = "<)k",
        .default_val = 0.5,
        .val_to_str = float_to_string,
    },
};

pat_state_pt pat_wave_init()
{
    pat_wave_state_t * state = malloc(sizeof(pat_wave_state_t));
    state->color = (color_t) {0.0, 0.0, 0.0, 0.0};
    state->phase = 0.0;
    state->type = OSC_SINE;
    state->last_t = 0;
    state->kx = 1.0;
    state->ky = 1.0;
    return state;
}

void pat_wave_del(pat_state_pt state)
{
    free(state);
}

void pat_wave_update(slot_t* slot, mbeat_t t)
{
    float k_mag;
    float k_ang;

    pat_wave_state_t* state = (pat_wave_state_t*)slot->state;
    state->color = param_to_color(slot->param_states[WAVE_COLOR].value);
    state->type = quantize_parameter(osc_quant_labels, slot->param_states[WAVE_TYPE].value);

    state->phase += MB2B(t - state->last_t) * power_quantize_parameter(slot->param_states[WAVE_OMEGA].value);
    state->phase = fmod(state->phase, 1.0); // Prevent losing float resolution
    state->last_t = t;

    k_mag = slot->param_states[WAVE_K_MAG].value * 2 + 0.2;
    k_ang = slot->param_states[WAVE_K_ANGLE].value * 2 * M_PI;
    state->kx = COS(k_ang) * k_mag;
    state->ky = SIN(k_ang) * k_mag;
}

void pat_wave_prevclick(slot_t * slot, float x, float y){
    pat_wave_state_t* state = (pat_wave_state_t*)slot->state;
    PARAM_UNUSED state;
    /* I don't know how I feel about resetting the state yet...
    state->phase = 0.;
    state->last_t = fmod(state->last_t, 16.0);
    */
    param_state_setq(&slot->param_states[WAVE_K_MAG], sqrt(pow(x, 2) + pow(y, 2)) / sqrt(2.0));
    param_state_setq(&slot->param_states[WAVE_K_ANGLE], (atan2(y, x) / (2.0 * M_PI)) + 0.5);
}

color_t pat_wave_pixel(slot_t* slot, float x, float y)
{
    pat_wave_state_t* state = (pat_wave_state_t*)slot->state;
    color_t result = state->color;
    result.a = osc_fn_gen(state->type, state->phase + y * state->ky + x * state->kx);
    return result;
}

pattern_t pat_wave = {
    .render = &pat_wave_pixel,
    .init = &pat_wave_init,
    .del = &pat_wave_del,
    .update = &pat_wave_update,
    .prevclick = &pat_wave_prevclick,
    .n_params = N_WAVE_PARAMS,
    .parameters = pat_wave_params,
    .name = "Wave",
};

// --------- Pattern: Bubble -----------

typedef struct
{
    color_t color;
    float r;
    float rho;
    float cx;
    float cy;
} pat_bubble_state_t;

enum pat_bubble_param_names {
    BUBBLE_COLOR,
    BUBBLE_R,
    BUBBLE_RHO,
    BUBBLE_CX,
    BUBBLE_CY,

    N_BUBBLE_PARAMS
};

parameter_t pat_bubble_params[N_BUBBLE_PARAMS] = {
    [BUBBLE_COLOR] = {
        .name = "Color",
        .default_val = 0.5,
    },
    [BUBBLE_R] = {
        .name = "r",
        .default_val = 0.5,
        .val_to_str = float_to_string,
    },
    [BUBBLE_RHO] = {
        .name = "\\rho",
        .default_val = 0.5,
        .val_to_str = float_to_string,
    },
    [BUBBLE_CX] = {
        .name = "cx",
        .default_val = 0.5,
        .val_to_str = float_to_string,
    },
    [BUBBLE_CY] = {
        .name = "cy",
        .default_val = 0.5,
        .val_to_str = float_to_string,
    },
};

pat_state_pt pat_bubble_init()
{
    pat_bubble_state_t * state = malloc(sizeof(pat_bubble_state_t));
    state->color = (color_t) {0., 0., 0., 0.};
    state->r = 0.;
    state->cx = 0.;
    state->cy = 0.;
    return state;
}

void pat_bubble_del(pat_state_pt state)
{
    free(state);
}

void pat_bubble_update(slot_t* slot, long t)
{
    PARAM_UNUSED t;
    pat_bubble_state_t* state = (pat_bubble_state_t*)slot->state;
    state->color = param_to_color(slot->param_states[BUBBLE_COLOR].value);
    state->r = slot->param_states[BUBBLE_R].value;
    state->rho = slot->param_states[BUBBLE_RHO].value * 1.3 + 0.3;
    state->cx = slot->param_states[BUBBLE_CX].value * 2 - 1.0;
    state->cy = slot->param_states[BUBBLE_CY].value * 2 - 1.0;
}

void pat_bubble_prevclick(slot_t * slot, float x, float y){
    // TODO: check that we have control of the param before writing to it
    slot->param_states[BUBBLE_CX].value = (x + 1.0) / 2;
    slot->param_states[BUBBLE_CY].value = (y + 1.0) / 2;
}

color_t pat_bubble_pixel(slot_t* slot, float x, float y)
{
    float d;
    pat_bubble_state_t* state = (pat_bubble_state_t*)slot->state;
    color_t result = state->color;

    d = sqrt(pow(state->cx - x, 2) + pow(state->cy - y, 2)) / state->r;
    
    if(d < 1.0)
        result.a = pow(1.0 - pow(d, state->rho), 1.0 / state->rho);
    else
        result.a = 0.0;
    return result;
}

pattern_t pat_bubble = {
    .render = &pat_bubble_pixel,
    .init = &pat_bubble_init,
    .del = &pat_bubble_del,
    .update = &pat_bubble_update,
    .prevclick = &pat_bubble_prevclick,
    .n_params = N_BUBBLE_PARAMS,
    .parameters = pat_bubble_params,
    .name = "Bubble",
};
