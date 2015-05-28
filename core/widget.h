#ifndef CORE_WIDGET_H
#define CORE_WIDGET_H

#include "ui/layout.h"
#include "core/time.h"
#include "core/err.h"
#include "util/math.h"
#include <stdarg.h>
#include <SDL/SDL.h>

typedef struct widget_class widget_class;
typedef struct widget       widget;

typedef int (*init_fn)  (widget * self, va_list ap                        );
typedef int (*start_fn) (widget * self                                    );
typedef int (*update_fn)(widget * self, mbeat_t t                         );
typedef int (*render_fn)(widget * self, SDL_Surface *onto, SDL_Rect *where);
typedef int (*event_fn )(widget * self, struct xy *pt, SDL_Event *evt     );
typedef int (*stop_fn  )(widget * self                                    );
typedef void(*deinit_fn)(widget * self);

struct widget_class {
  const char           *name;
  init_fn              init;
  start_fn             start;
  update_fn            update;
  render_fn            render;
  event_fn             event;
  stop_fn              stop;
  deinit_fn            deinit;
  size_t               class_data_size;
  void                *class_data;
  size_t               instance_data_size;
};

struct widget {
  widget_class        super;
  void               *instance_data;
  char               *name;
  widget             *parent;

  widget             *next;
  widget             *prev;

  widget             *child;
  size_t              nchildren;
  
  SDL_Rect            bbox;
};

widget  * widget_create(widget_class *cls, widget *parent, char *name, ... );
int       widget_start (widget *self );
int       widget_update(widget *self, mbeat_t t);
int       widget_render(widget *self, SDL_Surface *onto, SDL_Rect *where);
int       widget_post_event (widget *self, struct xy *pt, SDL_Event *evt);
int       widget_bubble_event(widget *root, struct xy *pt,SDL_Event *evt);
int       widget_stop  (widget *self );
void      widget_delete(widget **selfp);
#endif
