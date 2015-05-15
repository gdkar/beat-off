#ifndef CORE_CLS_H
#define CORE_CLS_H

#include <stdint.h>
#include <stddef.h>
typedef struct object object_t;
typedef void*         priv_t;
typedef priv_t        object_priv_t;
typedef priv_t        cls_priv_t;
typedef struct cls    cls_t;
typedef object_t* (*object_alloc_fn_pt) (const cls_t * _class);
typedef object_t* (*object_init_fn_pt)  (object_t * self);
typedef void      (*object_deinit_fn_pt)(object_t * self);
typedef int       (*object_update_fn_pt)(object_t * self, void *argv);
typedef void      (*object_free_fn_pt)  (object_t ** selfp);

object_t    *object_alloc(const cls_t *_class, const char *name);
object_t    *object_alloc_by_name(const char *_class, const char *name);
int          object_update(object_t *self, void *argv);
void         object_free(object_t **selfp);
typedef struct cls_funcs {
  object_alloc_fn_pt    alloc;
  object_init_fn_pt     init;
  object_update_fn_pt   update;
  object_deinit_fn_pt   deinit;
  object_free_fn_pt     free;
}cls_funcs_t;

typedef struct cls{
  const char *                 name;
  cls_t                       *next;
  const struct cls_funcs            *funcs;
  size_t                       obj_priv_size;
  object_priv_t                obj_priv_default;
  size_t                       cls_priv_size;
  cls_priv_t                   cls_priv;

}cls_t;

struct object{
  const struct cls             *cls;
  const struct cls_funcs       *funcs;
  char                   *name;
  size_t                  priv_size;
  object_priv_t           priv;
};


#endif
