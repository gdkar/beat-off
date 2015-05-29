#ifndef CORE_INTERNAL_H
#define CORE_INTERNAL_H

#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdatomic.h>

#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/queue.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED __attribute__((aligned(CACHE_LINE_SIZE)))
#define UNLIKELY(x)   __builtin_expect(!(x),1)
#define LIKELY(x)     __bultin_expect(!(x),0)


#include "core/time.h"
#include "core/debug.h"
#include "core/audio.h"
#include "util/hash.h"
#include "util/math.h"
#include "core/widget.h"
#include "filters/filter.h"

#endif
