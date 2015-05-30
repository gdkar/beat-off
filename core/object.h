#ifndef CORE_object_H
#define CORE_object_H

#include <sys/queue.h>
#include <stdbool.h>
#include <stdarg.h>
#include "core/time.h"
#include "core/debug.h"
#include <SDL2/SDL.h>

typedef struct object_class object_class;
typedef struct object       object;

typedef bool (*init_fn)   (object * self, va_list ap                                        );
typedef bool (*hittest_fn)(object * self, SDL_Point *pt                                     );
typedef bool (*event_fn  )(object * self,  SDL_Event *evt                                   );
typedef bool (*render_fn) (object * self, SDL_Renderer*ren,SDL_Texture *tex, SDL_Rect *where);
typedef void (*deinit_fn) (object * self                                                    );

struct object_class {
  const char           *name;
  init_fn              init;
  hittest_fn           hittest;
  event_fn             event;
  render_fn            render;
  deinit_fn            deinit;
  size_t               priv_size;
};
struct object {
  object_class       *cls;
  void               *priv;

  char               *name;
  object             *up;
  object             *left;
  object             *right;
  object             *down;;
  SDL_Rect            bbox;
};

void      object_set_root ( object *);
object  * object_create      (object_class *cls, object *parent, char *name, ...                   );
bool      object_event_send  (object *self, SDL_Event *evt                                         );
bool      object_event_bubble(SDL_Event *evt                                         );
bool      object_render      (object *self, SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *where   );
void      object_delete      (object *self                                                         );

#endif
