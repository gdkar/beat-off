#ifndef PROPERTIES_ENTITYPOOL_H
#define PROPERTIES_ENTITYPOOL_H

#include "core/beat-off.h"
#include "properties/array.h"
#include "util/util.h"
typedef struct entity { int64_t id;}entity;
typedef struct entitypool{
  HT64        emap;
  Array       data;
}entitypool;
typedef struct entitypool_elem{
  entity      ent;
}entitypool_elem;
#define entitypool_init(pool,Type) (entitypool_init_size(pool,sizeof(Type)))
static inline entitypool_init_size(entitypool *p, size_t item_size);
static void *entitypool_find(entitypool *p, entity ent);
static inline void entitypool_deinit(entitypool *p);
static void entitypool_remove(entitypool *p, entity ent);
static void *entitypool_add(entitypool *p, entity ent);
static inline entitypool_init_size(entitypool *p, size_t item_size);
{
  ht64_init(&p->emap);
  array_init_size(&p->data);
}
static inline void entitypool_deinit(entitypool *p)
{
  array_deinit(&p->data);
  ht64_deinit(&p->emap);
}
static void *entitypool_add(entitypool *p, entity ent)
{
  entitypool_elem *elem;
  if((elem=entitypool_find(p,ent.id))!=NULL)return elem;
  elem = array_add(p->array);
  elem->ent = ent;
  ht64_insert(&p->emap,ent,array_size(&p->data)-1);
  return elem;
}
static void entitypool_remove(entitypool *p, entity ent)
{
  off_t idx = ht64_find(&p->emap,ent.id);
  ht64_erase(&p->emap,ent.id);
  entitypool_elem *elem = array_remove(&p->data, idx);
  if(elem){ht64_insert(&p->emap,elem->ent,idx);}
}
static void *entitypool_find(entitypool *p, entity ent)
{
  off_t i = ht64_find(&p->emap,ent);
  return array_at(&p->data,i);
}
static void *entitypool_begin(entitypool *p){return array_begin(&p->data);}
static void *entitypool_end(entitypool *p){return array_end(&p->data);}
static off_t entitypool_size(entitypool *p){return array_size(&p->data);}
static void *entitypool_at(entitypool *p, off_t i){return array_at(&p->data,i);}
#endif
