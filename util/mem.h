#ifndef UTIL_MEM_H
#define UTIL_MEM_H
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static inline void freep_fn(void **pptr){
  if(pptr && *pptr){
    free(*pptr);
    *pptr = NULL;
  }
}
#define freep(_pptr) freep_fn((void**)_pptr)
#endif
