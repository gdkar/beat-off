#include "core/internal.h"

object * object_create ( object_class *cls, object *up, char *name, ... ){
  va_list ap;
  object *self = calloc(1,sizeof(object));
  self->cls  = cls;
  self->priv = calloc(1,cls->priv_size);
  self->name = name ? strdup(name):NULL;

  self->up   = up;
  self->down = NULL;
  if(!up || !up->down) {
    self->left = self;
    self->right = self;  
  }else{
    self->right = up->down;
    self->left  = self->right->left;
    self->left->right = self;
    self->right->left = self;
  }
  if(up && !up->down) 
    up->down = self;
  if(self->cls->init){
    va_start(ap,name);
     (self->cls->init(self, ap));
    va_end(ap);
  }
  return self; 
}
bool object_event_send( object *self, SDL_Event *evt){
  return (self && self->cls && self->cls->event)&&(self->cls->event(self, evt));
}

static object     * focus_object;
static object     * root;
static object * object_event_bubble_down (){
  SDL_Point pt, tpt;
  SDL_GetMouseState(&pt.x, &pt.y);
  tpt.x = pt.x - root->bbox.x;
  tpt.y = pt.y - root->bbox.y;
  object * next = root;
  object * curr = NULL;
  while(next){
    tpt.x = pt.x - next->bbox.x;
    tpt.y = pt.y - next->bbox.y;
    if ( next->cls->hittest ? next->cls->hittest(next,&tpt):in_rect(&tpt,&next->bbox)){
      curr = next;
      pt = tpt;
      next = curr->down;
    }else{
      next = next->left;
      if(next == curr->down) // we've gone all the way around the ring;
        break;
    }
  }
  DEBUG("new focus object is %s", curr->name);
  return curr;
}
static bool object_event_bubble_up ( object * obj, SDL_Event *evt){
  while(obj){
    if(obj->cls->event && obj->cls->event ( obj, evt ) )
      return true;
    else{
      if(obj==focus_object && obj->up){
        focus_object = obj->up;
      }
      obj = obj->up;
    }
  }
  return false;
}
bool object_event_bubble ( SDL_Event *evt){
  if(!evt) return false;
  if(evt->type == SDL_MOUSEBUTTONDOWN
  || evt->type == SDL_FINGERDOWN
  || evt->type == SDL_DROPFILE){
    focus_object =  object_event_bubble_down ( );
  }
  bool ret =  object_event_bubble_up( focus_object, evt );
  if(!focus_object && root)focus_object = root;
  DEBUG("current focus object is %s", focus_object->name);
  return ret;
}

bool object_render ( object *self, SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *where ){
  SDL_SetRenderTarget ( ren, tex );
  return ( self && self->cls->render && self->cls->render ( self, ren, tex, where ) );
}

void object_delete ( object *self ){
  if(self){
    while ( self->down ){
      object_delete ( self->down );
    }
    if(self->up && self->up->down == self ){
      if(self->right != self){
        self->up->down = self->right;
      }else {
        self->up->down = NULL;
      }
    }
    self->left->right = self->right;
    self->right->left = self->left;
    self->left        = self;
    self->right       = self;
    if(self->cls->deinit ) self->cls->deinit(self);
    if(self->priv){
      free(self->priv );
      self->priv = NULL;
    }
    if(self->name){
      free(self->name);
      self->name = NULL;
    }
    free(self);
  }
}
void object_set_root(object *new_root){root = new_root;}
