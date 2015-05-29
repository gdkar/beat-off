#include "core/internal.h"
#include "ui/boxlayout.h"


typedef struct boxlayout_priv{
  double vmargin;
  double hmargin;
  SDL_Color   stroke;
  SDL_Color   fill;
  SDL_Texture*texture;
}boxlayout_priv;

bool boxlayout_init(widget *self, va_list ap){
  boxlayout_priv * b = (boxlayout_priv*)self->instance_data;
    b->hmargin = va_arg(ap, long);
    b->vmargin  = va_arg(ap,long);
    b->stroke.r=va_arg(ap,long);
    b->stroke.g=va_arg(ap,long);
    b->stroke.b=va_arg(ap,long);
    b->stroke.a=va_arg(ap,long);
    b->fill.r=va_arg(ap,long);
    b->fill.g=va_arg(ap,long);
    b->fill.b=va_arg(ap,long);
    b->fill.a=va_arg(ap,long);
    self->location.x = va_arg(ap,long);
    self->location.y = va_arg(ap,long);
    self->location.w = va_arg(ap,long);
    self->location.h = va_arg(ap,long);
  return true;
}

bool boxlayout_hittest(widget *self, struct xy *pt){
  boxlayout_priv * b = (boxlayout_priv*)self->instance_data;
  SDL_Rect rect = self->location;
  rect.x += b->hmargin;
  rect.y += b->vmargin;
  rect.w -=2*b->hmargin;
  rect.h -=2*b->vmargin;
  bool ret =  in_rect(pt,&rect,NULL);
    if(ret)
      DEBUG("ignoring an event that nominally hit %s",self->name);
  return ret;
}

bool boxlayout_event(widget *self, SDL_Event *evt, va_list ap){
  (void) self;
  (void) evt;
  (void) ap;
  return false;
}

bool boxlayout_render(widget *self, SDL_Renderer *onto, SDL_Rect *where ){
  boxlayout_priv *b     = (boxlayout_priv *)self->instance_data;
  int  tw, th, tacc;
  uint32_t  tformat;
  if(b->texture ){
    SDL_QueryTexture(b->texture,&tformat,&tacc,&tw,&th);
    if(tw != self->location.w || th != self->location.h ||
        !(tacc&SDL_TEXTUREACCESS_TARGET)){
      SDL_DestroyTexture(b->texture);
      b->texture=NULL;
    }
  }
  if(!b->texture){
    DEBUG("creating texture with size %dx%d", self->location.w, self->location.h);
    b->texture = SDL_CreateTexture(onto, SDL_PIXELFORMAT_ARGB8888, 
      SDL_TEXTUREACCESS_TARGET,self->location.w, self->location.h);
    SDL_SetTextureBlendMode(b->texture,SDL_BLENDMODE_NONE);
  }
  SDL_Texture *original_target = SDL_GetRenderTarget(onto);
  SDL_SetRenderTarget(onto, b->texture);
  SDL_SetRenderDrawColor(onto, 0,0,0,0);
  SDL_RenderClear(onto);
  SDL_SetRenderDrawColor(onto, b->stroke.r,b->stroke.g, b->stroke.b, b->stroke.a);
  SDL_Rect rect = self->location;
  rect.x = 0; rect.y = 0;
  rect.x += b->hmargin;
  rect.y += b->vmargin;
  rect.w -=2*b->hmargin;
  rect.h -=2*b->vmargin;
  SDL_RenderFillRect(onto, &rect);
  SDL_SetRenderDrawColor(onto, b->fill.r,b->fill.g, b->fill.b, b->fill.a);
  rect.x += b->hmargin;
  rect.y += b->vmargin;
  rect.w -=2*b->hmargin;
  rect.h -=2*b->vmargin;
  SDL_RenderFillRect(onto, &rect);
  widget *child;
  CIRCLEQ_FOREACH( child, &self->children, siblings){
    //has_changed = has_changed || 
    widget_render(child, onto, &child->location);
  }
  SDL_SetRenderTarget(onto, original_target);
  rect = self->location;
  rect.x = 0;
  rect.y = 0;
  SDL_RenderCopy(onto, b->texture, NULL,where);
  return true;
}

void  boxlayout_deinit(widget *self){
  boxlayout_priv *b     = (boxlayout_priv *)self->instance_data;
   SDL_DestroyTexture(b->texture);
   b->texture = 0;
}

widget_class hbox_class = {
 .name="HBoxLayout"
,.init=boxlayout_init,
 .hittest=boxlayout_hittest
,.event  =boxlayout_event
,.render =boxlayout_render
,.deinit =boxlayout_deinit
,.class_data_size = 0
,.class_data = NULL
,.instance_data_size = sizeof(boxlayout_priv)};

widget_class vbox_class = {
 .name="VBoxLayout"
,.init=boxlayout_init,
 .hittest=boxlayout_hittest
,.event  =boxlayout_event
,.render =boxlayout_render
,.deinit =boxlayout_deinit
,.class_data_size = 0
,.class_data = NULL
,.instance_data_size = sizeof(boxlayout_priv)};

