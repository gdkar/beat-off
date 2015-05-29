#include "core/internal.h"
#include "ui/mainwin.h"
#include <SDL2/SDL_render.h>
typedef struct mainwin_priv{
  SDL_Window          *window;
  SDL_Renderer        *render;
  SDL_Texture         *texture;
  SDL_TimerID          render_loop;
  SDL_TimerCallback    render_callback;
  SDL_TimerID          update_loop;
  SDL_TimerCallback    update_callback;
}mainwin_priv;
uint32_t mainwin_render_callback ( uint32_t interval, void *vptr){
  widget *self = (widget*)vptr;
  mainwin_priv *m = (mainwin_priv*)self->instance_data;
  SDL_SetRenderTarget(m->render,NULL);
  widget_render(self, m->render, &self->location);
  SDL_RenderPresent(m->render);
  SDL_SetRenderDrawColor(m->render,255,255,255,255);
  SDL_RenderClear(m->render);
  return interval;
}
uint32_t mainwin_update_callback ( uint32_t interval, void *vptr){
  (void)vptr;
  return interval;
}
bool mainwin_init(widget *self, va_list ap){
  (void)ap;
  mainwin_priv *m     = (mainwin_priv *)self->instance_data;
  m->window    = SDL_CreateWindow("MainWindow",-1,-1,640,480,0);
  m->render    = SDL_CreateRenderer(m->window,-1,0);
  SDL_SetRenderDrawColor(m->render,128,128,128,255);
  SDL_RenderPresent(m->render);
  m->render_callback = &mainwin_render_callback;
  m->render_loop = SDL_AddTimer(1000./60., m->render_callback, (void*)self);
  m->update_callback = &mainwin_update_callback;
  m->update_loop = SDL_AddTimer(1000./60., m->update_callback, (void*)self);

  return true;
}

bool mainwin_hittest (widget *self, struct xy *pt){
  (void)self;
  (void)pt;
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
  return false;
}



bool mainwin_render (widget *self, SDL_Renderer *onto, SDL_Rect *where){
  (void)where;
  mainwin_priv *m     = (mainwin_priv *)self->instance_data;
  int w, h, tw, th, tacc;
 uint32_t  tformat;
  SDL_GetWindowSize(m->window,&w, &h);
//  bool has_changed = false;
  if(m->texture ){
    SDL_QueryTexture(m->texture,&tformat,&tacc,&tw,&th);
    if(tw < w || th < h ||(!(tacc&SDL_TEXTUREACCESS_STREAMING))){
      SDL_DestroyTexture(m->texture);
      m->texture=NULL;
    }
  }
  if(!m->texture){
    m->texture = SDL_CreateTexture(onto, SDL_PIXELFORMAT_ARGB8888, 
      SDL_TEXTUREACCESS_TARGET,w,h);
    SDL_SetTextureBlendMode(m->texture,SDL_BLENDMODE_NONE);
  }
  SDL_Texture *original_target = SDL_GetRenderTarget(onto);
  SDL_SetRenderTarget(onto, m->texture);
  SDL_SetRenderDrawColor(onto,0,0,0,255);
  SDL_RenderClear(onto);
  widget *child;
  CIRCLEQ_FOREACH( child, &self->children, siblings){
    widget_render(child, onto, &child->location);
  }
  SDL_SetRenderTarget(onto, original_target);
  SDL_RenderCopy(onto, m->texture, NULL,NULL);
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
