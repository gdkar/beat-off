#ifndef __ERR_H
#define __ERR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>

typedef enum log_level{
  LOG_FATAL = -1,
  LOG_ERROR =  0,
  LOG_WARN,  
  LOG_INFO,
  LOG_DEBUG,
  LOG_SPEW,
}log_level;
extern log_level g_log_level;
#define SPEW(x,...)  ((g_log_level>=LOG_SPEW)&&(fprintf(stderr,"SPEW: " __FILE__ " line %d: " x "\n", __LINE__, ##__VA_ARGS__)))
#define DEBUG(x,...)  ((g_log_level>=LOG_DEBUG)&&(fprintf(stderr,"DEBUG: " __FILE__ " line %d: " x "\n", __LINE__, ##__VA_ARGS__)))
#define INFO(x,...)  ((g_log_level>=LOG_INFO)&&(fprintf(stderr,"INFO: " __FILE__ " line %d: " x "\n", __LINE__, ##__VA_ARGS__)))
#define WARN(x,...)  ((g_log_level>=LOG_WARN)&&(fprintf(stderr,"WARNING: " __FILE__ " line %d: " x "\n", __LINE__, ##__VA_ARGS__)))
#define ERROR(x,...)  ((g_log_level>=LOG_ERROR)&&(fprintf(stderr,"ERROR: " __FILE__ " line %d: " x "\n", __LINE__, ##__VA_ARGS__)))
#define FATAL(x,...) ({fprintf(stderr,"FATAL: " __FILE__ " line %d: " x "\n", __LINE__, ##__VA_ARGS__);exit(EXIT_FAILURE);})

#define UNUSED(x) ((void)(x))

#endif
