#include "ui/ui_item.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <errno.h>
static ui_item_class *item_classes;

ui_item *ui_item_alloc_by_name(const char *name){
  ui_item_class *cls = item_classes;
  while(cls){
    if(!strcmp(cls->name,name))
      return ui_item_alloc(cls);
    cls=cls->next;
  }
  return NULL;
}
ui_item *ui_item_alloc(ui_item_class *cls){
  ui_item *item = cls->alloc();
  do{
    if(!item) break;
    if(!item->cls)    item->cls    = cls;
    if(!item->init)   item->init   = cls->init;
    if(!item->freep)   item->freep   = cls->freep;
    if(!item->render) item->render = cls->render;
    if(cls->priv_size && !item->priv){
      item->priv_size = cls->priv_size;
      item->priv      = malloc(item->priv_size);
      memset(item->priv,0,item->priv_size);
    }
    int ret = item->init(item);
    if(ret < 0)break;
    return item;
  }while(0);
  ui_item_free(item);
  return NULL;
}
void ui_item_free(ui_item *item){
  if(item){
    if(item->freep){item->freep(&item);}
    if(item&&item->priv) free(item->priv);
    if(item) free(item); 
  }
}
void ui_item_freep(ui_item**itemp){
  if(itemp){
    ui_item_free(*itemp);
    *itemp = NULL;
  }
}
int ui_item_render(ui_item *item, SDL_Renderer*r){
  if(item->render){
    return item->render(item,r);
  }else{
    return ENOSYS;
  }
}
int ui_item_event(ui_item *item, int type, void *evt){
  if(item && item->event){
    return item->event(item,type,evt);
  }else{
    return ENOSYS;
  }
}
