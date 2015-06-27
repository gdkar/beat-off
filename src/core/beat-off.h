#ifndef CORE_BEAT_OFF_H
#define CORE_BEAT_OFF_H


#ifdef __cplusplus
extern "C"{
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include "util/math.h"
#include "core/err.h"
#include "core/config.h"
#include "core/parameter.h"
#include "core/slot.h"
#include "ui/layout.h"
#include "util/color.h"
#include "util/string.h"
#include "audio/audio.h"
#include "ui/ui.h"
#include "output/output.h"
#include "midi/midi.h"
#include "dynamic/object.h"
#include "util/util.h"
#include <stdbool.h>
#include "util/tinycthread.h"



#define TYPED_ALLOC(x,n) do{(x) = (__typeof__(x))calloc(sizeof(*x),1);}while(0)
#define TYPED_REALLOC(x,n) ((__typeof__(x))realloc((x),n*sizeof(*x)))

#ifndef __cplusplus
#include <stdatomic.h>
#else 
};
#endif
#endif
