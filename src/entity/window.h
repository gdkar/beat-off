#ifndef ENTITY_WINDOW_H
#define ENTITY_WINDOW_H
#include "core/beat-off.h"
#include "entity/entity.h"

typedef struct renderable renderable;
typedef struct window_ctx window_ctx;

struct renderable{
  _Atomic (renderable    *)next;
  void (*render_fn)(window_ctx*);
  int  (*update_fn)(void *     );
  thrd_t update_thread;
};
struct window_ctx{
  GLFWwindow    *window;
  GLFWmonitor   *monitor;
  int            width;
  int            height;
  thrd_t         update_thread;
  thrd_t         events_thread;
  _Atomic (renderable    *)renderable_list;
};
typedef struct rect_geometry rect_geometry;
struct vec2{float x;float y;};
struct rect_geometry{
  entity_id id;
  uint32_t  hash;
  vec2 pos;
  vec2 size;
  float a,r,g,b;
};

#endif
