#include "core/cls.h"
#include "util/mem.h"
static const Class *cls_list_head = NULL;

Object *object_alloc(const Class * cls, const char *name){
  Object *o = NULL;
  if(cls) return 0;
  const ClassFns *fns = cls->fns;
  if(fns->alloc ){
    o = (*fns->alloc)(cls);
  }else{
    o           = calloc(sizeof(Object),1);
    o->priv_size= cls->obj_priv_size;
    o->priv     = cls->obj_priv_size?malloc(cls->obj_priv_size):0;
    if(cls->obj_priv_default){
      memmove(o->priv,cls->obj_priv_default,cls->obj_priv_size);
    }
  }
  o->cls      = cls;
  o->fns      = cls ? cls->fns   : NULL;
  o->name     = name?strdup(name): NULL;
  if(o->fns->init){
    (*o->fns->init)(o);
  }
  return o;
}
Object *object_alloc_by_name(const char *_class, const char *name){
  for(const  Class *cls_list = cls_list_head;cls_list;cls_list = cls_list->next){
    if(!strcmp(cls_list->name,_class)){
      return object_alloc(cls_list,name);
    }
  }
  return NULL;
}
int object_update(Object *self, void *argv){
  return (self && self->fns && self->fns->update)?(*self->fns->update)(self,argv):0;
}
void object_free(Object **selfp){
  if(selfp){
    Object *self         = *selfp;
    if(self->fns && self->fns->deinit){(*self->fns->deinit)(self);}
    if(self->name){freep((void**)&self->name);}
    if(self->fns && self->fns->free){(*self->fns->free)(selfp);self = selfp?*selfp:NULL;}
    if(self){
      if(self->priv) freep(&self->priv);
      freep((void**)selfp); 
    }
  }
}
