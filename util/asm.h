#ifndef LOL_ASM_H
#define LOL_ASM_H
#include <unistd.h>
#include <stdbool.h>
#include <x86intrin.h>
#include <stddef.h>
#include <stdint.h>

static inline uint64_t get_timestamp() {                                  
  uint32_t  hi,lo;
  asm volatile (                                                      
      "rdtsc"
      : "=d" (hi)
      , "=a" (lo)
      ); 
  return (((uint64_t)hi)<<32)|((uint64_t)lo);
}
static inline int ffs_int(int v) {
  int pos;
  asm volatile (
      "bsf %1, %0"
      : "=r" (pos)
      : "r" (v)
      );
  return pos;
}
static inline int ffs_int64(int v) {
  int pos;
  asm volatile (
      "bsfq %1, %0"
      : "=r" (pos)
      : "q" (v)
      );
  return pos;
}
static inline int fls_int(int v){
  int pos;
  asm volatile(
      "bsr %1, %0"
      : "=r" (pos)
      :  "r" (v)
      );
  return pos;
}
static inline int fls_int64(int64_t v){
  int pos;
  asm volatile(
      "bsrq %1, %0"
      : "=r" (pos)
      :  "q" (v)
      );
  return pos;
}
static inline bool bit_test_and_set(volatile void *base, int bit){
  bool ret = 0;
  asm volatile(
      "lock; bts %2, (%1);\n\t"
      "setnc %0"
    :  "=r"(ret)
    :   "q"(base)
    ,   "Ir"(bit)
    : "cc", "memory");
  return ret;
}
static inline bool bit_test_and_reset(volatile void *base, int bit){
  bool ret = 0;
  asm volatile(
      "lock; btr %2, (%1);\n\t"
      "setnc %0"
    :  "=r"(ret)
    :   "q"(base)
    ,   "Ir"(bit)
    : "cc", "memory");
  return ret;
}
#endif
