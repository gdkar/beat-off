#ifndef __ERR_H
#define __ERR_H

#include <stdlib.h>
#include <stdio.h>

#define _ERR_STRINGIFY2(x) #x
#define _ERR_STRINGIFY(x) _ERR_STRINGIFY2(x)

#define DEBUG(fmt,...) do{fprintf(stderr,"DEBUG: " __FILE__ " line " _ERR_STRINGIFY(__LINE__) ": " fmt "\n",##__VA_ARGS__);}while(0)
#define INFO(fmt,...) do{fprintf(stderr,"INFO: " __FILE__ " line " _ERR_STRINGIFY(__LINE__) ": " fmt "\n",##__VA_ARGS__);}while(0)
#define WARN(fmt,...) do{fprintf(stderr,"WARN: " __FILE__ " line " _ERR_STRINGIFY(__LINE__) ": " fmt "\n",##__VA_ARGS__);}while(0)
#define ERROR(fmt,...) do{fprintf(stderr,"ERROR: " __FILE__ " line " _ERR_STRINGIFY(__LINE__) ": " fmt "\n",##__VA_ARGS__);}while(0)

#define FAIL(...) {ERROR(__VA_ARGS__); exit(EXIT_FAILURE);}

#endif
