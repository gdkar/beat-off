#include "core/internal.h"

typedef struct window_priv {
  SDL_Window       *window;
  SDL_Renderer      *render;
  SDL_Color          bg;
  SDL_Texture       *texture;
  SDL_TimerID        redraw_timer;
  SDL_TimerCallback  redraw_callback;
}window_priv;

uint32_t window_redraw_callback(uint32_t interval, void *voidp){
  object *self = (object*) voidp;
  window_priv *p = (window_priv*)self->priv;
  DEBUG("In redraw callback");
  object_render(self, p->render, NULL, &self->bbox);
  return interval;
}
bool  window_render( object *self, SDL_Renderer *render, SDL_Texture *texture, SDL_Rect *where){
  window_priv *p = (window_priv*)self->priv;
  (void)texture;
  (void)where;
  SDL_SetRenderDrawColor(render, p->bg.r, p->bg.g, p->bg.b, p->bg.a);
  SDL_RenderClear(render);

  if(self->down){
    object *child = self->down;
    do{
      object_render ( child, p->render, NULL, &child->bbox);
      child=child->left;
    }while(child!=self->down);
  }
  SDL_RenderPresent(render);
  return true;
}
bool window_init ( object *self, va_list ap){
  window_priv *p = (window_priv *)self->priv;
  self->bbox.w = va_arg(ap, int);
  self->bbox.h = va_arg(ap, int);
  p->bg.r = va_arg(ap,int);
  p->bg.g = va_arg(ap,int);
  p->bg.b = va_arg(ap,int);
  p->bg.a = va_arg(ap,int);
  INFO("size %d x %d, color( %d, %d, %d, %d)", self->bbox.w, self->bbox.h, p->bg.r, p->bg.g, p->bg.b, p->bg.a);
  if(self->bbox.w <= 0 || self->bbox.h <= 0){
    self->bbox.w = 640;
    self->bbox.h = 480;
  }
  p->window  = SDL_CreateWindow(self->name, -1, -1, self->bbox.w, self->bbox.h,0);
  if(!p->window)
    return false;
  p->render  = SDL_CreateRenderer(p->window,-1,0);
  if(!p->render){
    return false;
    ERROR("failed to create a renderer");
    }
//  p->texture = SDL_CreateTexture(p->render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, self->bbox.h, self->bbox.w);
  return true;
  p->redraw_callback = window_redraw_callback;
  p->redraw_timer = SDL_AddTimer(1000./10, p->redraw_callback, (void*)self);
}
bool window_hittest ( object *self, SDL_Point *pt){
  return in_rect(pt,&self->bbox);
}

bool window_event ( object *self, SDL_Event *evt){
  window_priv *p = (window_priv *)self->priv;
  INFO("%s received a type %d", self->name, evt->type);
  if(evt->type == SDL_QUIT){
    SDL_Quit();
    exit(0);
  }else if(evt->type == SDL_WINDOWEVENT){
    switch (evt->window.event ){
      case SDL_WINDOWEVENT_EXPOSED:
      case SDL_WINDOWEVENT_SHOWN:{
          if(!p->redraw_timer){
            p->redraw_timer = SDL_AddTimer(1000./10, window_redraw_callback, (void*)self);
          return true; 
          }
        }
      case SDL_WINDOWEVENT_HIDDEN :{
        if(p->redraw_timer){
//          SDL_RemoveTimer(p->redraw_timer);
//          p->redraw_timer = 0;
          return true;
        }
      }
      case SDL_WINDOWEVENT_RESIZED :{
      if(self->down){
          object *child = self->down;
          do{
            object_event_send(child,evt);
            child = child->left;
          }while(child!=self->down);
        }
      return true;
      }
      case SDL_WINDOWEVENT_CLOSE :{
        return true;
      }
    }
  }
  return false;
}
void window_deinit(object *self ){
  window_priv *p = (window_priv *)self->priv;
  INFO("Window deinit");
  if(p->texture){
    SDL_DestroyTexture(p->texture);
    p->texture = NULL;
  }
  if(p->redraw_timer){
    SDL_RemoveTimer(p->redraw_timer);
    p->redraw_timer = 0;
  }
  if(p->render)SDL_DestroyRenderer(p->render);
  if(p->window) SDL_DestroyWindow(p->window);
}

object_class window_class = { .name = "Window Object",
  .init = window_init,
  .hittest = window_hittest,
  .event   = window_event,
  .render  = window_render,
  .deinit  = window_deinit,
  .priv_size = sizeof(window_priv)
};
