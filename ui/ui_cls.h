#ifndef UI_UI_CLS_H
#define UI_UI_CLS_H
#include "core/cls.h"
typedef struct UiClass UiClass;
typedef struct UiObject UiObject;
typedef struct UiEvent UiEvent;
typedef struct UiEventType UiEventType;
typedef int (*ui_object_start_fn_pt) (UiObject *self);
typedef int (*ui_object_stop_fn_pt)  (UiObject *self);
typedef int (*ui_object_event_fn_pt) (UiObject *self, UiEvent *evt );
typedef int (*ui_object_render_fn_pt)(UiObject *self, UiEvent *evt );

int ui_object_start (UiObject *self);
int ui_object_stop  (UiObject *self);
int ui_object_event (UiObject *self, UiEvent *evt);
int ui_object_render(UiObject *self, UiEvent *evt);

typedef struct UiEventType{
  const char            *name;
  const char            *format; /* argument format for use with sscanf */
}UiEventType;

struct UiEvent{
  UiEventType           *type;
  const char             data[];   /* argument data for use with sscanf */
};

typedef struct UiClassFns{
  Class                  super;
  ui_object_start_fn_pt  start;
  ui_object_stop_fn_pt   stop;
  ui_object_event_fn_pt  event;
  ui_object_render_fn_pt render;
}UiClassFns;

struct UiClass{
  const char                *name;
  Class                     *next;
  const UiClassFns          *fns;
  size_t                     obj_priv_size;
  ObjectPriv                 obj_priv_default;
  size_t                     cls_priv_size;
  ClassPriv                  cls_priv;
};

struct UiObject{
  const UiClass             *cls;
  const UiClassFns          *fns;
  char                      *name;
  size_t                     priv_size;
  ObjectPriv                 priv;
};
#endif
