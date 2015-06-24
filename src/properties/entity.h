#ifndef PROPERTIES_ENTITY_H
#define PROPERTIES_ENTITY_H
#include "core/beat-off.h"
#include "properties/entitypool.h"

static entity entity_nill = { .id=0 };
static _Atomic(off_t) latest_entity = 1;
typedef struct existspool_elem
{
  entitypool_elem base;
  bool            destroyed;
}existspool_elem;
struct destroyedpool_elem
{
  entitypool_elem base;
  off_t           pass;
}destroyedpool_elem;
static entitypool exists_pool;
static entitypool destroyed_pool;
static Array      destroyed_array;
static entity _generate_id()
{
  return (entity){ atomic_fetch_add(&latest_entity)};
}
static entity entity_create()
{
  entity ent = generate_id();
  entitypool_add(&existspool,ent);
  return ent;
}
static void entity_remove(entity ent)
{
  (destroyedpool_elem*)entitypool_add(&destroyed_pool,ent)->pass = 1;
}
static void entity_destroy(entity ent)
{
  if(!entitypool_get(&exists_pool,ent)) return;
  if(entitymap_get  (&destroyed_pool,ent)) return;
  entitypool_remove (&exists_pool,ent);
  entity_remove(ent);
}
static bool entity_destroyed(ent)
{
  return entitymap_get(&destroyed_pool,ent);
}
static void entity_init()
{
  entitypool_init(&exists_pool,existspool_elem);
  entitypool_init(&destroyed_pool,destroyedpool_elem);
}
static void entity_deinit()
{
  entitymap_deinit(&exits_pool);
  entitymap_deinit(&destroyed_pool);
}
#endif
