#ifndef CORE_WIDGET_H
#define CORE_WIDGET_H

#include <sys/queue.h>
#include <stdbool.h>
#include <stdarg.h>
#include "ui/layout.h"
#include "core/time.h"
#include "core/debug.h"
#include <SDL/SDL.h>

typedef struct widget_class widget_class;
typedef struct widget       widget;

typedef bool (*init_fn)   (widget * self, va_list ap                        );
typedef bool (*hittest_fn)(widget * self, struct xy *pt                     );
typedef bool (*event_fn  )(widget * self,  SDL_Event *evt, va_list ap       );
typedef bool (*render_fn) (widget * self, SDL_Renderer*onto, SDL_Rect *where );
typedef void (*deinit_fn) (widget * self);

struct widget_class {
  const char           *name;
  init_fn              init;
  hittest_fn           hittest;
  event_fn             event;
  render_fn            render;
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

  CIRCLEQ_ENTRY(       widget) siblings;
  CIRCLEQ_HEAD (qhead, widget) children;
  
  SDL_Rect            location;
};

widget  * widget_create      (widget_class *cls, widget *parent, char *name, ... );
bool      widget_post_event  (widget *self, SDL_Event *evt, ...                  );
bool      widget_event_filter(widget *root, struct xy *pt, SDL_Event *evt, ...   );
bool      widget_render      (widget *self, SDL_Renderer*onto, SDL_Rect *where   );
void      widget_delete      (widget **selfp                                     );

#endif
