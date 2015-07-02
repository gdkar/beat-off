#ifndef __PATTERN_H
#define __PATTERN_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/time.h"
#include "util/color.h"
#include "core/parameter.h"
#ifdef __cplusplus
#define __restruct__
#endif
struct slot;
typedef enum pat_command_status
{
    STATUS_START,  // e.g. Mouse down; MIDI note on
    STATUS_CHANGE, // e.g. Mouse drag; MIDI aftertouch
    STATUS_STOP,   // e.g. Mouse up; MIDI note off
} pat_command_status_t;
typedef struct pat_command
{
    int index;
    pat_command_status_t status;
    float value;
} pat_command_t;
typedef void* pat_state_pt;
typedef void (*pat_init_fn_pt)(pat_state_pt);
typedef void (*pat_update_fn_pt)(struct slot* slot, mbeat_t t);
typedef void (*pat_command_fn_pt)(struct slot* slot, pat_command_t cmd);
typedef color_t (*pat_render_fn_pt)(const void * __restrict__ state, float x, float y);
typedef void (*pat_render_img_fn_pt)(const void * __restrict__ state, float alpha, float *xs,float*ys,size_t n,color_t*out);
typedef struct pattern
{
    pat_init_fn_pt init;
    pat_update_fn_pt update;
    pat_command_fn_pt command;
    pat_render_fn_pt render;
    pat_render_img_fn_pt render_img;
    GLuint           gl_prog;
    int n_params;
    const parameter_t* parameters;
    int state_size;
    const char* name;
} pattern_t;
extern int n_patterns;
extern pattern_t **patterns;
#define MAKE_PATTERN_RENDER_IMG_FN\
  static color_t render_fn(const state_t *restrict state, float x, float y){return render(state,x,y);}\
  static void render_img_fn(const void * __restrict__ state,float a, float *xs,float*ys,size_t n,color_t*out)\
{\
  out = __builtin_assume_aligned(out,16);\
  if(a>1.f){\
    for(size_t j = 0; j < n; j++){\
      color_t c = render(state,xs[j],ys[j]);\
      out[j].r = c.r*c.a;\
      out[j].g = c.g*c.a;\
      out[j].b = c.b*c.a;\
      out[j].a = 1.f*c.a;\
    }\
  }else{\
    for(size_t j = 0; j < n; j++){\
      color_t c = render(state,xs[j],ys[j]);\
      c.a*=a;\
      const float oma = 1.f-c.a;\
      out[j].r = out[j].r * oma + c.r*c.a;\
      out[j].g = out[j].g * oma + c.g*c.a;\
      out[j].b = out[j].b * oma + c.b*c.a;\
      out[j].a = out[j].a * oma + 1.f*c.a;\
    }\
  }\
}
#define MAKE_PATTERN { \
    .init = (pat_init_fn_pt)&init, \
    .update = &update, \
    .command = &command, \
    .render = (pat_render_fn_pt)&render_fn, \
    .render_img = (pat_render_img_fn_pt)&render_img_fn,\
    .n_params = N_PARAMS, \
    .parameters = params, \
    .state_size = sizeof(state_t), \
    .name = name, \
}
#endif
