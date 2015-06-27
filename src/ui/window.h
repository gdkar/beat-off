#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include "core/beat-off.h"


GLuint  make_shader(GLenum type,const char *text);
GLuint  make_shader_program(const char *types,...);

#endif
