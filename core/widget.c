#include "core/widget.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

widget * widget_create ( widget_class *cls, widget *parent, char *name, ... ){
  va_list ap;
  widget *self = calloc(1,sizeof(widget));
  memmove(&self->super, cls, sizeof(*cls));
  self->instance_data = malloc(cls->instance_data_size);
  memset(self->instance_data,0,cls->instance_data_size);

  self->parent = parent;
  if(parent && parent->child){
    widget *sibling = parent->child;
    self->prev = sibling->prev;
    self->next = sibling;
    self->next->prev = self;
    self->prev->next = self;
    parent->nchildren++; 
  }else if(parent){
    parent->child     = self;
    self->next = self;
    self->prev = self;
    parent->nchildren++; 
  }else{
    self->next = self;
    self->prev = self;
  }
  if(self->super.init){
    va_start(ap,name);
    self->super.init(self, ap);
    va_end(ap);
  }
  return self; 
}
int widget_start ( widget *self ){return (self && self->super.start )?self->super.start(self):0;}
int widget_update( widget *self, mbeat_t t){return (self && self->super.update) ? self->super.update(self,t):0;}
int widget_render( widget *self, SDL_Surface *onto, SDL_Rect *where){
  return (self && self->super.render ) ? self->super.render(self, onto, where):0;
}
int widget_post_event ( widget *self, struct xy *pt, SDL_Event *evt){
  return (self && self->super.event)?self->super.event(self, pt, evt ):0;
}
int widget_bubble_event ( widget *root, struct xy *pt, SDL_Event *evt){
  struct xy rel_pt, cur_pt;
  if(root && in_rect(pt, &root->bbox, &rel_pt)){
    widget *child = root->child;
    widget *next  = child;
    cur_pt = rel_pt;
    do{
      if(in_rect(&cur_pt, &next->bbox, &rel_pt)){
        cur_pt = rel_pt;
        root = next;
        child= next
             =  root->child;
        if(child) continue;
      }
    }while(next && ((next = next->next)!=child));
    int done = 0;
    while(root && !done){
      done = (root->super.event && root->super.event(root, &cur_pt, evt));
      cur_pt.x += root->bbox.x;
      cur_pt.y += root->bbox.y;
      root = root->parent;
    }
    return done;
  }
  return 0;
}
int widget_stop( widget *self ){return (self && self->super.stop)?self->super.start(self):0;}

void widget_delete(widget ** selfp){
  if(!selfp)return;
  widget *self = *selfp;
  while(self->nchildren){
    widget *child = self->child;
    widget_delete( &child);
  }
  if(self->parent){
    if(self->next == self){
      self->parent->nchildren = 0;
      self->parent->child     = NULL;
    }else{
      if(self->parent->child == self)
        self->parent->child = self->prev;
      self->prev->next = self->next;
      self->next->prev = self->prev;
      self->parent->nchildren --;
      self->next = self;
      self->prev = self;
    }
  }else{
    if(self->next!=self){
      self->prev->next = self->next;
      self->next->prev = self->prev;
      self->next = self;
      self->prev = self;
    }
  }
  if(self->super.deinit) self->super.deinit(self);
  self->parent = NULL;
  if(self->instance_data)
    free(self->instance_data);
  self->instance_data = NULL;
  if(self->name) free(self->name);
  self->name = NULL;
  free(self);
  *selfp = NULL;
}
