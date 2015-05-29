#ifndef __FILTER_H
#define __FILTER_H

#include <SDL2/SDL.h>
typedef struct filter filter;
struct vamp_plugin;
typedef struct vamp_plugin *vamp_plugin_p;
typedef struct filter {
    // Input parameters
//    int n_params;
//    parameter_t* parameters;
//    param_state_t* param_states;

    // Output value
//    param_output_t output;

    // VAMP Configuration
    char * vamp_so;
    char * vamp_id;
    unsigned int vamp_output;
    vamp_plugin_p vamp_plugin;

    // UI Elements
    char * name;
    SDL_Color color;
    char display;

    // State
//    filter_state_pt state;
//    graph_state_t graph_state;

    // Function Calls
//    filter_init_fn_pt init;
//    filter_update_fn_pt update;
//    filter_del_fn_pt del;
} filter;


#endif
