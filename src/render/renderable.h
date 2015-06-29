#pragma once

#include "render/drawaable.h"

#include "core/beat-off.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <json-c/json.h>

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdatomic.h>

#define poffsetof(type,field)((void*)(&((0*)0)->field))

static GLuint make_shader(GLenum type, const char *fn)
{
  if(!fn) return 0;
  FILE *ifile = fopen(fn,"r");
  fseek(ifile,0,SEEK_END);
  off_t size = ftell(ifile);
  rewind(ifile);
  char *ifile_data = (char*)malloc(size);
  off_t fill = 0;
  do{
    off_t chunk = fread(ifile_data+fill,1,size-fill,ifile);
    if(chunk<=0)break;
    fill += chunk;
  }while(fill<size);
  fclose(ifile);
  GLuint shader = glCreateShader(type);
  if(!shader){free ifile_data;return 0;}
  glShaderSource(shader, 1, (const GLchar**)&ifile_data,NULL);
  free(ifile_data);
  glCompileShader(shader);
  GLint status;
  glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
  INFO("compilation of shader %s %s successful.\n",fn,status?"was":"was not");
  if(!status){
    char log[256];
    glGetShaderInfoLog(shader,sizeof(log),NULL,log);
    WARN("compilation log:\n\n %s\n",log);
    glDeleteShader(shader);shader=0; 
  }
  return shader;
}
static GLuint make_program(const char *vert, const char *geom, const char *frag){
  GLuint prog  = glCreateProgram();
  if(!prog)return 0;
  GLuint _vert = make_shader(GL_VERTEX_SHADER,vert);
  if(_vert)glAttachShader(prog,_vert);
  GLuint _geom = make_shader(GL_GEOMETERY_SHADER,geom);
  if(_geom)glAttachShader(prog,_geom);
  GLuint _frag = make_shader(GL_FRAGMENT_SHADER,frag);
  if(_frag)glAttachShader(prog,_frag);
  glLinkProgram(prog);
  GLint status;
  glGetProgramiv(prog,GL_LINK_STATUS,&status);
  iINFO("linkage of program for \n%s\n%s\n%s\n was %ssuccessful\n",status?"":"not ");
  if(!status){
      char log[256];
      glGetProgramInfoLog(shader,sizeof(log),NULL,log);
      WARN("linkage log:\n\n %s\n",log);
      glDeleteProgram(prog);prog=0; 
  }
  glDeleteShader(_vert);
  glDeleteShader(_geom);
  glDeleteShader(_frag);
  return prog;
  
}
typedef struct vec2{
  float x;float y;
}  vec2;
typedef struct vec3{
  float x;float y;float z;
}vec3;
typedef struct vec4{
  float x;float y;float z;float w;
}vec4;
typedef struct ivec2{
  int x;int y;
}ivec2;
typedef struct ivec3{
  int x; int y;int z;
}ivec3;
typedef struct ivec4{
  int x;int y;int z;int w;
}ivec4;

#define bufferData(target,data,len,usage) glBufferData((target),(len)*sizeof(*data),(void*)(data),(usage))

typedef struct gl_state{
  int          a_type; // type ( passed as attribute )
  int          a_index;
  int          a_parent;
  int          a_depth;
  vec2         a_pos;
  vec2         a_size;
  vec4         a_color;
  vec4         a_user0;
}gl_state;

typedef struct aux_state{
  vec2         a_rel_pos;
  size_t       a_priv_size;
  void        *a_priv_data;
}aux_state;
typedef struct renderable renderabale;
typedef struct render_type{
  const char  *name;
  int          typecode;
  GLuint       program;
  size_t       nb_shaders;
  GLuint      *shaders;
  size_t       nb_textures;
  GLuint      *textures;
  bool       (*initialize)(render_type *self);
  void       (*cleanup)   (render_type *self);
  void       (*update)(renderable *rnd,render_type *type, gl_state *gstate, aux_state *astate, int len);
  void       (*render)(renderable *rnd, render_type *type);
  bool       (*key_event)(gl_state *gstate, aux_state *astate, SDL_Event *evt);
  bool       (*mouse_event)(gl_state *gstate, aux_state *astate, SDL_Event *ev);
  bool       (*add_instance)(render_type *type, gl_state *gstate, aux_state *astate);
  bool       (*del_instance)(render_type *type, gl_state *gstate, aux_state *astate);
}render_type;

struct renderable{
  struct json_object *config;
  size_t         nb_items
  gl_state      *items;
  aux_state     *items_aux;
  size_t         nb_items_buffered;
  GLuint         vao;
  GLuint         vbo;
  
  GLuint         fbo_render;
  GLuint         fbo_hitmap;

  SDL_Window    *window;
  DL_GLContext   context;

  size_t         nb_renderers;
  render_type  **renderers;
};
renderable  *renderable_new(struct json_object *config);
void         renderable_del(renderable *rnd);
int          renderable_add_renderer(renderable *rnd, render_type *type);
gl_state    *renderable_add_item(renderable *rnd, int type);
void         renderable_del_item(renderable *rnd, gl_state *state);
void         renderable_update(renderable *rnd);
void         renderable_render(renderable *rnd);
bool         renderable_poll  (renderable *rnd);
