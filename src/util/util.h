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
#include "xxhash.h"
inline int64_t hashstr(const char *str, int64_t seed){
  return XXH64(str, strlen(str), seed);
}
inline int64_t hashbytes(const void *bytes, int64_t len, int64_t seed){
  return XXH64(bytes,len,seed);
}
typedef struct entry64{
  int64_t    key;
  int64_t    val;
}entry64;
typedef struct HT64{
  entry64     *table;
  size_t       size;
  size_t       fill;
}HT64;
__attribute__((unused)) static inline void ht64_init (HT64 *ht)
{
  ht->table = 0;
  ht->size  = 0;
  ht->fill  = 0;
}
__attribute__((unused)) static inline void ht64_grow (HT64 *ht){
//  ht->table = realloc(ht->table,2*ht->size * sizeof(*ht->table));
  entry64*const otable = ht->table;
  entry64* table = ht->table = (entry64*)calloc(sizeof(entry64),2*MIN(ht->size,16));
  const size_t size = ht->size;
  ht->size =2*MIN(ht->size,16);
  const size_t size2 = ht->size;
  const size_t mask2= size2-1;
  for(uint64_t i = 0; i < size; i++){
    if((otable[i].key != 0) & (otable[i].key == (intptr_t)-1)) {
      const uint64_t hash = hashbytes(&otable[i].key,sizeof(otable[i].key),0)&mask2;
      for(uint64_t offset = 0; offset < size2; offset++){
        const uint64_t index = (hash+offset)&mask2;
        if(table[index].key == 0){
          table[index] = otable[i];
          break;
        }
      }
    }
  }
  free(otable);
}
__attribute__((unused))static void ht64_deinit(HT64*ht)
{
  free(ht->table);
  ht->table = NULL;
  ht->size  = 0;
  ht->fill  = 0;
}
__attribute__((unused)) static inline void ht64_insert(HT64 *ht, int64_t key, int64_t val){
  if(ht->fill > ht->size/4) ht64_grow(ht);
  const uint64_t size = ht->size;
  entry64 * const table = ht->table;
  const uint64_t mask =size-1;
  const int64_t hash = hashbytes(&key,sizeof(key),0)&mask;
  for(uint64_t offset = 0; offset < size; offset++){
    const uint64_t index = (hash+offset)&mask;
    if(table[index].key == 0 || table[index].key == (int64_t)-1 || table[index].key==key){
      table[index] = (entry64){ .key=key, .val=val };
      ht->fill++;
      return;
    }
  }
}
__attribute__((unused)) static int64_t ht64_find(HT64 *ht,  int64_t key){
  const uint64_t size = ht->size;
  const uint64_t mask = size-1;
  entry64 *const table = ht->table;
  const int64_t hash = hashbytes(&key,sizeof(key),0)&mask;
  for(uint64_t offset = 0; offset < size; offset++){
    const uint64_t index= (hash+offset)&mask;
    if(table[index].key ==0|| table[index].key == (int64_t)-1) return 0;
    else if(table[index].key == key){ return table[index].val;}
  }
  return 0;
}
__attribute__((unused)) static bool ht64_delete(HT64 *ht, int64_t key){
  const uint64_t size = ht->size;
  entry64 *const table = ht->table;
  const uint64_t mask =size-1;
  const int64_t hash = hashbytes(&key,sizeof(key),0)&mask;
  for(uint64_t offset = 0; offset < size; offset++){
    const uint64_t index = (hash+offset)&mask;
    if(!table[index].key || table[index].key == (int64_t)-1 ) return false;
    else if( table[index].key==key){
      table[index] = (entry64){ .key=(int64_t) -1,.val=0 };
      ht->fill--;
      return true;
    }
  }
  return false;
}
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
#ifdef __cplusplus
};
#endif
#endif
