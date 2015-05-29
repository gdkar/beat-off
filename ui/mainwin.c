#include "core/internal.h"
#include "ui/mainwin.h"
#include <SDL2/SDL_render.h>
typedef struct mainwin_priv{
  SDL_Window      *window;
  SDL_Renderer    *render;
  SDL_Texture     *texture;
  SDL_TimerID      watchdog;
}mainwin_priv;

bool mainwin_init(widget *self, va_list ap){
  (void)ap;
  mainwin_priv *m     = (mainwin_priv *)self->instance_data;
  m->window    = SDL_CreateWindow("MainWindow",-1,-1,640,480,0);
  m->render    = SDL_CreateRenderer(m->window,-1,0);
  SDL_SetRenderDrawColor(m->render,0,0,0,0);
  SDL_RenderClear       (m->render);
  SDL_RenderPresent(m->render);
  return true;
}

bool mainwin_hittest (widget *self, struct xy *pt){
  (void)self;
  (void)pt;
  SPEW("got a hit at location %dx%d", pt?pt->x:-1,pt?pt->y:-1);
  return true;
}
bool mainwin_event   (widget *self, SDL_Event *evt, va_list ap){
  (void)self;
  (void)ap;
  if(evt->type == SDL_QUIT){
    WARN("received QUIT request");
    SDL_Quit();
    exit(0);
  }
  else
    DEBUG("MainWindow received an event of type %d", evt->type);
  return false;
}

bool mainwin_render (widget *self, SDL_Surface *onto, SDL_Rect *where){
  mainwin_priv *m     = (mainwin_priv *)self->instance_data;
  if(!m->texture)m->texture = SDL_CreateTextureFromSurface(m->render, onto);
  SDL_UpdateTexture(m->texture,NULL,onto->pixels, onto->pitch);
  SDL_RenderCopy(m->render,m->texture,where,NULL);
  SDL_RenderPresent(m->render);
  return true;
}

void mainwin_deinint(widget *self){
  mainwin_priv *m     = (mainwin_priv *)self->instance_data;
  SDL_DestroyTexture(m->texture);
  SDL_DestroyRenderer(m->render);
  SDL_DestroyWindow  (m->window);
}
widget_class mainwin_class = {
  .name = "MainWindow",
  .init = mainwin_init,
  .hittest = mainwin_hittest,
  .event   = mainwin_event,
  .render  = mainwin_render,
  .class_data_size = 0,
  .class_data      = NULL,
  .instance_data_size = sizeof(mainwin_priv)};
