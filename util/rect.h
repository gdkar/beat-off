#ifndef UTIL_RECT_H
#define UTIL_RECT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <stdbool.h>

static inline bool  in_rect ( SDL_Point *pt, SDL_Rect *rect ){
  return  (rect->x <= pt->x) && ((rect->x+rect->w) >= pt->x) 
    && (rect->y <= pt->y) && ((rect->y+rect->h) >= pt->y);
}

static inline void point_add ( SDL_Point *to, SDL_Point *what){
  to->x+=what->x;
  to->y+=what->y;
}
static inline void point_sub (SDL_Point  *from,SDL_Point *what){
  from->x-=what->x;
  from->y-=what->y;
} 
static inline void point_scale ( SDL_Point *pt, double factor){
  pt->x *= factor;
  pt->y *= factor;
}
static inline SDL_Point point_make_relative ( SDL_Point *pt, SDL_Rect *rect ){
  return (SDL_Point){.x = pt->x - rect->x, .y = pt->y - rect->y};
}
static inline SDL_Point point_make_absolute ( SDL_Point *pt, SDL_Rect *rect ){
  return (SDL_Point){ .x = pt->x + rect->x, .y = pt->y + rect->y };
}
#endif
