#include "core/internal.h"

widget * widget_create ( widget_class *cls, widget *parent, char *name, ... ){
  va_list ap;
  widget *self = calloc(1,sizeof(widget));
  memmove(&self->super, cls, sizeof(*cls));
  self->instance_data = calloc(1,cls->instance_data_size);

  self->parent = parent;
  CIRCLEQ_INIT(&self->children);
  if(self->parent ){
    CIRCLEQ_INSERT_TAIL(&parent->children, self, siblings);
  }
  if(self->super.init){
    va_start(ap,name);
    bool ret = (!self->super.init(self, ap));
    va_end(ap);
    if(!ret) widget_delete(&self);
  }
  return self; 
}
//bool widget_hittest( widget *self, struct xy *pt)
//  {return (self && self->super.update &&  self->super.hittest(self,pt));}
bool widget_render( widget *self, SDL_Surface *onto, SDL_Rect *where)
  {return (self && self->super.render && self->super.render(self, onto, where));}
bool widget_post_event ( widget *self,  SDL_Event *evt, ...){
  va_list ap;
  if(self && self->super.event){
    va_start(ap,evt);
    bool ret = self->super.event(self, evt, ap);
    va_end(ap);
    return ret;
  }
  return false;
}
bool widget_event_filter( widget *root, struct xy *pt, SDL_Event *evt, ...){
  if(!root || !root->super.hittest || !root->super.hittest(root,pt))
    return false;
  while(root){
    widget *child;
    CIRCLEQ_FOREACH( child, &root->children, siblings ){
      if ( child->super.hittest && child->super.hittest(child, pt )){
        root = child;
        break;
      }
    }
    break;
  }
  va_list ap;
  va_start(ap, evt);
  bool handled = false;
  while(root && !handled){
    if(root->super.event){
      va_list aq;
      va_copy(aq, ap);
      handled = root->super.event(root, evt, aq);
      va_end(aq);
    }
    root = root->parent;
  }
  va_end(ap);
  return handled;
}
void widget_delete(widget ** selfp){
  if(!selfp)return;
  widget *self = *selfp;
  widget *child;
  CIRCLEQ_FOREACH_REVERSE(child,&self->children,siblings){
    widget *temp = child;
    widget_delete(&temp);
  }
  if(self->parent){
    CIRCLEQ_REMOVE(&self->parent->children,self, siblings);
  }
  if(self->super.deinit) self->super.deinit(self);
  self->parent = NULL;
  if(self->instance_data){
    free(self->instance_data);
    self->instance_data = NULL;
  }
  if(self->name){
    free(self->name);
    self->name = NULL;
  }
  free(self);
  *selfp = NULL;
}
