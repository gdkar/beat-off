extern "C"{
#include "core/beat-off.h"
#include "entity/entity.h"
}
#include <unordered_map>
#include <vector>

struct attribute{
/*  struct hash_record {
    entity_id       id;
    uint32_t        index;
  };
  struct dense_record{
    entity_id       id;
    uint32_t        hash;
  };*/
  size_t                                m_item_size;
  size_t                                m_item_count;
  std::unordered_map<uint32_t, hash_record> m_hash;
  std::vector<char>                     m_data;
  attribute(size_t size):m_item_size(size){}
  
  dense_record *at(entity_id ent)
  {
    hash_record &hrec = m_hash.at((uint32_t)hashfn(ent));
    return reinterpret_cast<dense_record*>(&m_data[m_item_size * hrec.index]);
  }
  bool insert(dense_record *drec)
  {
    drec->hash = (uint32_t)hashfn(drec->id);
    if(m_hash.count(drec->hash))
    {
      hash_record &hrec = m_hash.at(drec->hash);
      memmove(&m_data[m_item_size*hrec.index],drec,m_item_size);
      return true;
    }else{
      hash_record hrec = (hash_record){drec->id,m_hash.size()};
      m_hash.insert(std::make_pair(drec->hash,hrec));
      m_data.resize(m_hash.size()*m_item_size);
      memmove(&m_data[m_item_size*hrec.index],drec,m_item_size);
      return true;
    }
    return false;
  }
  bool erase(entity_id ent)
  {
    uint32_t hash_val = (uint32_t)hashfn(ent);
    if(!m_hash.count(hash_val)) return false;
    hash_record &hrec = m_hash.at(hash_val);
    dense_record *drec = reinterpret_cast<dense_record*>(&m_data[m_item_size * (m_hash.size()-1)]);
    memmove(&m_data[m_item_size*hrec.index],drec,m_item_size);
    m_hash.at(drec->hash).index = hrec.index;
    m_hash.erase(hash_val);
    return true;
  } 
};

extern "C"{
  attribute *attribute_alloc_(size_t item_size){
    return new attribute(item_size);
  }
  void attribute_free(attribute *att){
    delete att;
  }
  dense_record *attribute_lookup(attribute *att, entity_id ent)
  {
    return att->at(ent);
  }
  bool attribute_insert(attribute *att, dense_record *rec)
  {
    return att->insert(rec);
  }
  bool attribute_erase(attribute *att, entity_id ent)
  {
    return att->erase(ent);
  }
}
