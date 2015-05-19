#ifndef UTIL_HASH_H 
#define UTIL_HASH_H 
#ifdef __cplusplus
extern "C" {
#endif
#include <stdatomic.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "core/defs.h"

static inline uint64_t rdtsc(){
  uint32_t hi, lo;
  asm volatile (
      "rdtsc"
    : "=d" (hi)
    , "=a" (lo)
    );
  return (((uint64_t)hi)<<32)|((uint64_t)lo);
}
static inline int ffs32(int v){
  int pos;
  asm volatile (
      "bsf %1, %0"
    : "=r"(pos)
    :  "r"(v)
    );
  return pos;
}
static inline int ffs64(int64_t v){
  int pos;
  asm volatile (
    "bsfq %1, %0"
  :  "=r" (pos)
  :   "q" (v)
      );
  return pos;
}
#define ffs(_v) ((sizeof(_v)<8)?ffs32((int)_v):ffs64((int64_t)_v))
static inline int fls32(int v){
  int pos;
  asm volatile (
      "bsr %1, %0"
    : "=r"(pos)
    :  "r"(v)
    );
  return pos;
}
static inline int fls64(int64_t v){
  int pos;
  asm volatile (
    "bsrq %1, %0"
  :  "=r" (pos)
  :   "q" (v)
      );
  return pos;
}
#define fls(_v) ((sizeof(_v)<8)?fls32((int)_v):fls64((int64_t)_v))
inline uint64_t hashfn(uint64_t k){ /* modified from the murmurhash3 - 64bit finalizer */
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
__attribute__((unused)) static bool _ht64_lookup(uint64_t *table, uint32_t size, uint64_t value){
  const uint32_t mask = size-1;
  const uint32_t hash = hashfn(value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index= (hash+offset)&mask;
    if(!table[index]) return false;
    else if(table[index] == value) return true;
  }
  return false;
}
#define ht64_lookup(_t, _s, _v) _ht64_lookup(_t,(uint32_t)_s,(uint64_t)_v)
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
__attribute__((unused)) static inline bool _ht32_lookup(uint32_t *table, uint32_t size, uint32_t value){
  const uint32_t mask = size-1;
  const uint32_t hash =hashfn (value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index= (hash+offset)&mask;
    if(!table[index]) return false;
    else if(table[index] == value) return true;
  }
  return false;
}
#define ht32_lookup(_t, _s, _v) _ht32_lookup(_t,(uint32_t)_s,(uint32_t)_v)

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
uint32_t threadrand_minstd();
#ifdef __cplusplus
};
#endif
#endif
