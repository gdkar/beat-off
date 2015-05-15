#ifndef UI_UI_CLS_H
#define UI_UI_CLS_H
#include "core/cls.h"
typedef struct ui_cls ui_cls_t;
typedef struct ui_object ui_object_t;
typedef struct ui_evt ui_evt_t;
typedef int (*ui_object_start_fn_pt) (ui_object_t *self);
typedef int (*ui_object_stop_fn_pt)  (ui_object_t *self);
typedef int (*ui_object_event_fn_pt) (ui_object_t *self, ui_evt_t *evt );
typedef int (*ui_object_render_fn_pt)(ui_object_t *self, ui_evt_t *evt );

int ui_object_start (ui_object_t *self);
int ui_object_stop  (ui_object_t *self);
int ui_object_event (ui_object_t *self, ui_evt_t *evt);
int ui_object_render(ui_object_t *self, ui_evt_t *evt);

typedef struct ui_evt_type{
  const char            *name;
  const char            *priv_fmt; /* argument format for use with sscanf */
}ui_evt_type_t;

struct ui_evt{
  struct ui_evt_type    *type;
  const char             data[];   /* argument data for use with sscanf */
};

typedef struct ui_cls_funcs {
  struct cls             super;
  ui_object_start_fn_pt  start;
  ui_object_stop_fn_pt   stop;
  ui_object_event_fn_pt  event;
  ui_object_render_fn_pt render;
}ui_cls_funcs_t;

struct ui_cls{
  const char                *name;
  cls_t                     *next;
  const struct ui_cls_funcs *funcs;
  size_t                     obj_priv_size;
  object_priv_t              obj_priv_default;
  size_t                     cls_priv_size;
  cls_priv_t                 cls_priv;
};

struct ui_object {
  const struct ui_cls       *cls;
  const struct ui_cls_funcs *funcs;
  char                      *name;
  size_t                     priv_size;
  object_priv_t              priv;
};
#endif
