#ifndef CORE_BEAT_OFF_H
#define CORE_BEAT_OFF_H


#ifdef __cplusplus
extern "C"{
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
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
#include "dynamic/dynamic.h"
#include "util/util.h"
#include <stdbool.h>
#include "util/tinycthread.h"
#ifndef __cplusplus
#include <stdatomic.h>
#else 
};
#endif
#endif
