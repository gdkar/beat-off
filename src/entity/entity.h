#ifndef ENTITY_ENTITY_H
#define ENTITY_ENTITY_H

#include "core/beat-off.h"

typedef uint32_t  entity_id;

#define MAX_HASHTAB_POPULATION 4096
#define MIN_DENSE_RESERVATION    16
typedef struct hash_record{
  entity_id           id;
  uint32_t            index;
//  uint16_t            offset;
}hash_record;
typedef struct dense_record{
  entity_id           id;
  entity_id           hash;
}dense_record;
typedef struct attribute attribute;
/*typedef struct attribute{
  entity_hash_record      hashtable[MAX_HASHTAB_POPULATION];
  size_t                  current_size;
  size_t                  current_fill;
  size_t                  item_size;
  char*                   vector;
}attribute;*/

attribute *attribute_alloc_(size_t item_size);
#define attribute_alloc(type) attribute_alloc_(sizeof(type))
void      attribute_free (attribute *att);
dense_record  *attribute_lookup(attribute *att, entity_id ent);
bool  attribute_erase (attribute *att, entity_id ent);
bool  attribute_insert(attribute *att, dense_record *rec);


#endif
