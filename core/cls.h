#ifndef CORE_CLS_H
#define CORE_CLS_H

#include <stdint.h>
#include <stddef.h>
typedef struct Object   Object;
typedef struct Class    Class;
typedef struct ClassFns ClassFns;
typedef void*         Priv;
typedef Priv          ObjectPriv;
typedef Priv          ClassPriv;
typedef Object* (*object_alloc_fn_pt) (const Class * cls);
typedef Object* (*object_init_fn_pt)  (Object * self);
typedef void      (*object_deinit_fn_pt)(Object * self);
typedef int       (*object_update_fn_pt)(Object * self, void *argv);
typedef void      (*object_free_fn_pt)  (Object ** selfp);

Object    *object_alloc(const Class *cls, const char *name);
Object    *object_alloc_by_name(const char *cls, const char *name);
int          object_update(Object *self, void *argv);
void         object_free(Object **selfp);
struct ClassFns{
  object_alloc_fn_pt    alloc;
  object_init_fn_pt     init;
  object_update_fn_pt   update;
  object_deinit_fn_pt   deinit;
  object_free_fn_pt     free;
};

struct Class{
  const char *                 name;
  const Class                 *next;
  const ClassFns              *fns;
  size_t                       obj_priv_size;
  ObjectPriv                   obj_priv_default;
  size_t                       cls_priv_size;
  ClassPriv                    cls_priv;
};

struct Object{
  const Class                   *cls;
  const ClassFns                *fns;
  char                          *name;
  size_t                         priv_size;
  ObjectPriv                     priv;
};
#endif
