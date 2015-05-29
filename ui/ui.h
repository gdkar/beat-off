#ifndef __UI_H
#define __UI_H

#include "ui/layout.h"


extern void (*mouse_drag_fn_p)(struct xy);
extern void (*mouse_drop_fn_p)();

void ui_init();
void ui_quit();
void ui_update_master();
void ui_render();
int ui_poll();

#endif
