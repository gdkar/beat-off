#include <string.h>
#include "core/err.h"
#include "ui/ui.h"
#include "ui/text.h"
#include "ui/layout.h"
#include "ui/layout_constants.h"
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>

static struct txt * last_txt = 0;

static void lh_table_surface_dtor(struct lh_entry *ent)
{
  free(ent->k);
  SDL_Surface *s = (SDL_Surface*)ent->v;
  SDL_FreeSurface((SDL_Surface*)s);
}
void text_load_font(struct txt * params){
    char * filename = strcatdup(config.path.fonts, params->font);
    if(!filename) return;
    params->ui_font.font = TTF_OpenFont(filename, params->size);
    if(!params->ui_font.font) FAIL("TTF_OpenFont Error: %s\n", SDL_GetError());
    free(filename);
    params->ui_font.next = last_txt;
    params->cache = lh_kchar_table_new(64,"layout cache",&lh_table_surface_dtor);
    last_txt = params;
}
void text_unload_fonts(){
    while (last_txt){
        TTF_CloseFont(last_txt->ui_font.font);
        last_txt = last_txt->ui_font.next;
    }
}
void text_render(SDL_Surface * surface, rect_t *where, struct txt * params, const SDL_Color * color, const char * text){
    // Passing TTF_RenderText_*(...) a NULL text pointer results in "undefined behavior"
    if(!text) return; 
    if(!params->ui_font.font) text_load_font(params);
    // Passing TTF_RenderText_*(...) a NULL text pointer results in a segfault.
    // Its better to not draw anything than to segfault. If we couldn't load the font, just give up.
    if(!params->ui_font.font) return;
    if(!color)  color = &params->color;
    char buf[128];
    snprintf(buf,sizeof(buf),"%hhd%hhd%hhd %s",color->r,color->g,color->b,text);
    struct lh_entry *entry = lh_table_lookup_entry(params->cache,buf);
    SDL_Surface *cached_surface=NULL;
    if(entry){cached_surface = (SDL_Surface*)entry->v;}
    if(!cached_surface){
      WARN("Cache miss! %s\n",buf);
      SDL_Surface* msg;
#ifdef TEXT_ANTIALIAS
      msg = TTF_RenderText_Blended(params->ui_font.font, text, *color);
#else
      msg = TTF_RenderText_Solid(params->ui_font.font, text, *color);
#endif
      if(!msg) return;
      lh_table_insert(params->cache,strdup(buf),msg);
      cached_surface=msg;
    }
    rect_t r;
    r.x = params->x;
    r.y = params->y;
    r.w = cached_surface->w;
    r.h = cached_surface->h;
    r.x+=where->x;
    r.y+=where->y;
    switch(params->align){
        case LAYOUT_ALIGN_BR: r.y -= r.h; // fall through
        case LAYOUT_ALIGN_TR:r.x -= r.w;break;
        case LAYOUT_ALIGN_CC:r.y -= r.h / 2; // fall through
        case LAYOUT_ALIGN_TC:r.x -= r.w / 2;break;
        case LAYOUT_ALIGN_TL:
        default:break;
    }
    SDL_BlitSurface(cached_surface, 0, surface, &r);
    struct lh_entry *tmp;
    lh_foreach_safe(params->cache,entry,tmp){
      if(lh_table_length(params->cache) > 4096){
        lh_table_delete_entry(params->cache,entry);
      }
    }
}
