#ifndef LOL_UTIL_H
#define LOL_UTIL_H
#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
#include "util/asm.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
#include "util/murmur3_32_constexpr.hpp"
#endif
#include "util/MurmurHash3.h"
static inline uint64_t hashfn(uint64_t k){
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



static inline uint64_t threadrand_128(){
  static __thread uint64_t s[2];
  if(__builtin_expect((!s[0]&&!s[1]),0)){
    s[0] = (uint64_t)s;
    s[1] = get_timestamp();
    uint64_t s1 = s[ 0 ];
    const uint64_t s0 = s[ 1 ];
    s[ 0 ] = s0;
    s1 ^= s1 << 23; // a
    s[ 1 ] = ( s1 ^ s0 ^ ( s1 >> 17 ) ^ ( s0 >> 26 ) ) ; // b, c
  }
  uint64_t s1 = s[ 0 ];
	const uint64_t s0 = s[ 1 ];
	s[ 0 ] = s0;
	s1 ^= s1 << 23; // a
	return ( s[ 1 ] = ( s1 ^ s0 ^ ( s1 >> 17 ) ^ ( s0 >> 26 ) ) ) + s0; // b, c

}
static inline uint64_t threadrand_1024(){
  static __thread uint64_t s[16];
  static __thread int      index = 0;
  int p = (index = (index+1)&15);;
  if(__builtin_expect(!s[p],0)){
    for(int i = 0; i< 16; i++)
      s[i] = threadrand_128();
  }
  uint64_t s0 = s[0];
  uint64_t s1 = s[p];
  s1 ^=(s1 << 31);
  s1 ^=(s1 >> 11);
  s0 ^=(s0 >> 30);
  return (s[ p ] = s0 ^ s1 ) * 1181783497276652981LL;
}
#ifndef __cplusplus
static inline uint32_t mmhash(const char *const data)
{
  uint32_t out;
  const uint32_t seed = 0;
  MurmurHash3_x86_32(data, strlen(data), seed, &out);
  return out;
}
#endif
#ifdef __cplusplus
};
constexpr inline uint32_t mmhash(const char *const key ) noexcept
{
  return Murmur3_32(key,0);
}
#endif
#endif
