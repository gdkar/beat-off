#ifndef __UI_H
#define __UI_H

#include "core/beat-off.h"
#include "core/slot.h"
#include "ui/layout.h"


typedef struct ui_context{
  SDL_Window *window;
  SDL_Renderer *render;
  SDL_GLContext context;
  struct layout *layout;
}ui_context;

extern ui_context *uic;

extern param_state_t * active_param_source;

extern struct xy mouse_drag_start;
extern struct xy mouse_drag_delta;

extern void (*mouse_drag_fn_p)();
extern void (*mouse_drop_fn_p)();

__attribute__((unused)) static void ui_start(void (*ui_done)()){}
__attribute__((unused)) static void ui_stop(){}

#define HANDLED 1
#define UNHANDLED 0
#define PROPAGATE(x) if(x == HANDLED){return HANDLED;}

SDL_Surface * ui_create_surface_or_die(int width, int height);
ui_context *ui_create_context(struct layout *layout);
void ui_delete_context(ui_context *c);
#endif
