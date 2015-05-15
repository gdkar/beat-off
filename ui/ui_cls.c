#include "ui/ui_cls.h"

int ui_object_start(UiObject *self){
  return (self && self->fns && self->fns->start)?(*self->fns->start)(self):0;
}
int ui_object_stop(UiObject *self){
  return (self && self->fns && self->fns->stop)?(*self->fns->stop)(self):0;
}
int ui_object_event(UiObject *self, UiEvent *evt){
  return (self && self->fns && self->fns->event)?(*self->fns->event)(self,evt):0;
}
int ui_object_render(UiObject *self, UiEvent *evt){
  return (self && self->fns && self->fns->render)?(*self->fns->render)(self,evt):0;
}
