#ifndef CORE_DEFS_H
#define CORE_DEFS_H
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <unistd.h>

#include "core/err.h"

#define LIKELY(x) __builtin_expect(!!(x),1)
#define UNLIKELY(x) __builtin_expect(!!(x),0)
#define CACHE_LINE_SIZE 64
#define CACHE_ALIGN     __attribute__((aligned(CACHE_LINE_SIZE)))

#define DECL_NOINLINE   __attribute__((noinline))
#define DECL_INLINE     __attribute__((force_inline))
#define DECL_UNUSED     __attribute__((unused))

#define PARAM_UNUSED     (void)

#endif
