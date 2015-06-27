#ifndef __UI_TEXT_H__
#define __UI_TEXT_H__

#include "ui/ui.h"
#include "ui/layout.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

void text_load_font(struct txt * params);
void text_unload_fonts();
void text_render(rect_t *where,  struct txt * params, const SDL_Color * color, const char * text);

#endif
