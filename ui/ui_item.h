#ifndef UI_UI_ITEM_H
#define UI_UI_ITEM_H
#include <stdint.h>
#include <stddef.h>
#include <SDL2/SDL.h>
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct ui_item_class ui_item_class;
typedef struct ui_item ui_item;
struct ui_item_class{
  const char *name;
  ui_item_class *next;
  ui_item *(*alloc)();
  int (*init)(ui_item*);
  void(*freep)(ui_item**);
  int(*render)(ui_item*,SDL_Renderer *);
  int(*event)(ui_item*,int,void*);
  size_t priv_size;
  void*  priv;
};

struct ui_item{
  const char  *name;
  ui_item     *parent;
  ui_item     *children;
  int          nchildren;
  SDL_Rect     rect;
  SDL_Texture *tex;
  ui_item_class*cls;
  int (*init)  (ui_item *);
  void(*freep)  (ui_item **);
  int (*render)(ui_item *,SDL_Renderer *);
  int (*event )(ui_item *,int type, void*evt);
  size_t       priv_size;
  void*        priv;
};

ui_item   *ui_item_alloc(ui_item_class * cls);
ui_item   *ui_item_alloc_by_name(const char *name);
void       ui_item_free(ui_item*);
void       ui_item_freep(ui_item**);
int        ui_item_render(ui_item*,SDL_Renderer*);
int        ui_item_event (ui_item*,int,void*);

#endif
