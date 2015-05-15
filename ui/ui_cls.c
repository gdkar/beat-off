#include "ui/ui_cls.h"

int ui_object_start(ui_object_t *self){
  return (self && self->funcs && self->funcs->start)?(*self->funcs->start)(self):0;
}
int ui_object_stop(ui_object_t *self){
  return (self && self->funcs && self->funcs->stop)?(*self->funcs->stop)(self):0;
}
int ui_object_event(ui_object_t *self, ui_evt_t *evt){
  return (self && self->funcs && self->funcs->event)?(*self->funcs->event)(self,evt):0;
}
int ui_object_render(ui_object_t *self, ui_evt_t *evt){
  return (self && self->funcs && self->funcs->render)?(*self->funcs->render)(self,evt):0;
}
