#ifndef LOL_UTIL_H
#define LOL_UTIL_H
#ifdef __cplusplus
#include "init.h"
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "asm.h"
#include "core/internal.h"

inline uint64_t hashfn(uint64_t k){
  k ^= (k>> 31);
  k *= 0x7fb5d329728ea185;
  k ^= (k>> 27);
  k *= 0x81dadef4bc2dd44d;
  k ^= (k>> 33);
  return k;
}
__attribute__((unused)) static inline void _ht64_insert(uint64_t *table, uint32_t size, uint64_t value){
  const uint32_t mask =size-1;
  const uint32_t hash = hashfn(value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index = (hash+offset)&mask;
    if(!table[index] || table[index]==value){
      table[index] = value;
      return;
    }
  }
}
#define ht64_insert(_t,_s,_v) _ht64_insert(_t,(uint32_t)_s,(uint64_t)_v)
__attribute__((unused)) static bool _ht64_find(uint64_t *table, uint32_t size, uint64_t value){
  const uint32_t mask = size-1;
  const uint32_t hash = hashfn(value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index= (hash+offset)&mask;
    if(!table[index]) return false;
    else if(table[index] == value) return true;
  }
  return false;
}
#define ht64_find(_t, _s, _v) _ht64_find(_t,(uint32_t)_s,(uint64_t)_v)

__attribute__((unused)) static inline void _ht32_insert(uint32_t *table, uint32_t size, uint32_t value){
  const uint32_t mask =size-1;
  const uint32_t hash = hashfn(value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index = (hash+offset)&mask;
    if(!table[index] || table[index]==value){
      table[index] = value;
      return;
    }
  }
}
#define ht32_insert(_t,_s,_v) _ht32_insert(_t,(uint32_t)_s,(uint32_t)_v)
__attribute__((unused)) static inline bool _ht32_find(uint32_t *table, uint32_t size, uint32_t value){
  const uint32_t mask = size-1;
  const uint32_t hash =hashfn (value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index= (hash+offset)&mask;
    if(!table[index]) return false;
    else if(table[index] == value) return true;
  }
  return false;
}
#define ht32_find(_t, _s, _v) _ht32_find(_t,(uint32_t)_s,(uint32_t)_v)

#define next_pow_2(x) ({ __typeof__(x) __x__ = (x)-1;\
    __x__ |= __x__>>1;\
    __x__ |= __x__>>2;\
    __x__ |= __x__>>4;\
    if(sizeof(__x__)>1)__x__|= __x__>>8;\
    if(sizeof(__x__)>2)__x__|= __x__>>16;\
    if(sizeof(__x__)>4)__x__|= ((uint64_t)__x__)>>32;\
    __x__+1;})

typedef struct {
  uint64_t  s[2];
}xorshift128_state_t;

typedef struct {
  uint64_t    s[16];
  int         index;
}xorshift1024_state_t __attribute__((aligned(CACHE_LINE_SIZE)));

uint64_t threadrand_128();
uint64_t threadrand_1024();
#ifdef __cplusplus
};
#endif
#endif
