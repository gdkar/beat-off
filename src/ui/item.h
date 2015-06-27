#pragma once
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <iterator>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <random>
#include <functional>
#include <utility>
#include <sys/mman.h>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <unistd.h>
#define poffsetof(type, field) \
       ((void *) (&((type *) 0)->field))

using namespace std;

typedef struct vec2{
  float x;float y;
}vec2;
typedef struct vec3{
  float x; float y; float z;
}vec3;
typedef struct vec4{
  float x; float y; float z; float w;
}vec4;
typedef struct item_gl_state{
  vec4  a_bounds;
  int   a_depth;
  int   a_type;
  int   a_index;
  int   a_parent;
  vec4  a_color;
  vec4  a_user0;
}item_gl_state; /* 64 bytes, one cache line */
typedef struct item_aux_state{
  vec2    m_rel_box;
  size_t  m_priv_size;
  void   *m_priv_data;
}item_aux_state;


struct batch_renderer{
  GLuint            m_geom;
  GLuint            m_vert;
  GLuint            m_frag;
  GLuint            m_prog;
  vector<GLuint>    m_tex;
  static GLuint   compileShader(GLenum type, const char *filename)
  {
    if(!filename) return 0;
    FILE *ifile = fopen(filename,"r");
    fseek(ifile,0,SEEK_END);
    off_t size = ftell(ifile);
    rewind(ifile);
    char *source = (char*)malloc(size);
    char *wptr = source;
    do{
      ssize_t chunk = fread(wptr, 1,(source+size-wptr),ifile);
      if(chunk>0){
        wptr += chunk;
      }
    }while(source+size > wptr);
    fclose(ifile);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader,1,(const GLchar**)&source,nullptr);
    glCompileShader(shader);
    GLint status;
    char log[512];
    glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
    fprintf(stderr,"Compilation of shader %s successful\n",status?"was":"wasn't");
    glGetShaderInfoLog(shader,sizeof(log),NULL,log);
    fprintf(stderr,"Shader compilation log:\n%s\n\n",log);
    free(source);
    return shader;
  }
  explicit batch_renderer(const char *geom = nullptr,const char *vert = nullptr,const char *frag = nullptr){
    m_geom = compileShader(GL_GEOMETRY_SHADER,geom);
    m_vert = compileShader(GL_VERTEX_SHADER,vert);
    m_frag = compileShader(GL_FRAGMENT_SHADER, frag);
    m_prog = glCreateProgram();
    glAttachShader(m_prog,m_vert);
    glAttachShader(m_prog,m_geom);
    glAttachShader(m_prog,m_frag);
    glBindFragDataLocation(m_prog,0,"f_out");
    glLinkProgram(m_prog);
  }
  virtual ~batch_renderer(){
    glDeleteProgram(m_prog);
    glDeleteShader(m_frag);
    glDeleteShader(m_vert);
    glDeleteShader(m_geom);
  }
  virtual void  update();
};
struct renderer_registry{
  ui_context             *m_context;
  vector<item_gl_state>   m_gl_state;
  vector<item_aux_state>  m_aux_state;
  unordered_map<int,int>  m_id_to_index;
  GLuint                  m_vao;
  GLuint                  m_ibo;
  GLuint                  m_vbo;
  GLuint                  m_fbo;
  GLuint                  m_fbo_hit;
  vector<batch_renderer>  m_render_types;
  renderer_registry( ui_context *ctx)
  :m_context(ctx) 
  {
//    SDL_GL_MakeCurrent(m_context->window,m_context->context);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1,&m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
  }
  void uploadState()
  {
    if(m_gl_state.size()){
      glBindVertexArray(m_vao);
      glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
      glBufferData(GL_ARRAY_BUFFER, m_gl_state.size()*sizeof(m_gl_state[0]),&m_gl_state[0], GL_STREAM_DRAW);
    }

  }
  void bindRenderer(batch_renderer &br)
  {
    glUseProgram(br.m_prog);
    GLuint location;
    location = glGetAttribLocation(br.m_prog,"a_bounds");
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE,
        sizeof(item_gl_state),poffsetof(item_gl_state,a_bounds));
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,0);

    location = glGetAttribLocation(br.m_prog,"a_depth");
    glVertexAttribPointer(location, 1, GL_INT, GL_FALSE,
        sizeof(item_gl_state),poffsetof(item_gl_state,a_depth));
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,0);

    location = glGetAttribLocation(br.m_prog,"a_type");
    glVertexAttribPointer(location, 1, GL_INT, GL_FALSE,
        sizeof(item_gl_state),poffsetof(item_gl_state,a_type));
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,0);

    location = glGetAttribLocation(br.m_prog,"a_index");
    glVertexAttribPointer(location, 1, GL_INT, GL_FALSE,
        sizeof(item_gl_state),poffsetof(item_gl_state,a_index));
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,0);

    location = glGetAttribLocation(br.m_prog,"a_parent");
    glVertexAttribPointer(location, 1, GL_INT, GL_FALSE,
        sizeof(item_gl_state),poffsetof(item_gl_state,a_parent));
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,0);

    location = glGetAttribLocation(br.m_prog,"a_color");
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE,
        sizeof(item_gl_state),poffsetof(item_gl_state,a_color));
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,0);

    location = glGetAttribLocation(br.m_prog,"a_user0");
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE,
        sizeof(item_gl_state),poffsetof(item_gl_state,a_user0));
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,0);
  }
};
