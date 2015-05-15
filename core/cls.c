#include "core/cls.h"
#include "util/mem.h"
static const struct cls *cls_list_head = NULL;

object_t *object_alloc(const struct cls * _class, const char *name){
  object_t *o = NULL;
  if(_class) return 0;
  const struct cls_funcs *_funcs = _class->funcs;
  if(_funcs->alloc ){
    o = (*_funcs->alloc)(_class);
  }else{
    o           = calloc(sizeof(object_t),1);
    o->priv_size= _class->obj_priv_size;
    o->priv     = _class->obj_priv_size?malloc(_class->obj_priv_size):0;
    if(_class->obj_priv_default){
      memmove(o->priv,_class->obj_priv_default,_class->obj_priv_size);
    }
  }
  o->cls      = _class;
  o->funcs    = _class ? _class->funcs : NULL;
  o->name     = name?strdup(name):NULL;
  if(o->funcs->init){
    (*o->funcs->init)(o);
  }
  return o;
}
object_t *object_alloc_by_name(const char *_class, const char *name){
  for(const struct cls  *cls_list = cls_list_head;cls_list;cls_list = cls_list->next){
    if(!strcmp(cls_list->name,_class)){
      return object_alloc(cls_list,name);
    }
  }
  return NULL;
}
int object_update(object_t *self, void *argv){
  return (self && self->funcs && self->funcs->update)?(*self->funcs->update)(self,argv):0;
}
void object_free(object_t **selfp){
  if(selfp){
    object_t *self         = *selfp;
    if(self->funcs && self->funcs->deinit){(*self->funcs->deinit)(self);}
    if(self->name){freep((void**)&self->name);}
    if(self->funcs && self->funcs->free){(*self->funcs->free)(selfp);self = selfp?*selfp:NULL;}
    if(self){
      if(self->priv) freep(&self->priv);
      freep((void**)selfp); 
    }
  }
}
