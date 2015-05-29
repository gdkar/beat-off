#ifndef __CORE_TIME_H__
#define __CORE_TIME_H__

#include <time.h>

typedef double PaTime;

#define ts2time(x) ((double)(x.tv_sec + (x.tv_nsec*1e-9)))
#define time2ts(x) \
({ long intpart = (long)(x);\
   long fract= (long)(1e9 *  (x)-whole);\
    (struct timespec){ .tv_sec  = whole, \
                       .tv_nsec = fract};\
 })

#endif // __CORE_TIME_H__
