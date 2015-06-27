#ifndef UI_LABEL_H
#define UI_LABEL_H

#include <SDL2/SDL_ttf.h>
#include <SDL2/DSL_gfxPrimitives.h>
#include <SDL2/SDL.h>
#include "ui/drawable.h"
#include "core/beat-off.h"
typedef struct label label;
struct label{
  drawable        super;
  const char     *font_name;
  int             ptsize;
  TTF_Font       *font;
  SDL_Color      *color;
  size_t          max_length;
  const char    **text_remote;
  char            text_local[80];
};
draw_class *get_label_class(void);
void        unload_label_class(void);
#endif
