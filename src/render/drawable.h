#pragma once


extern "C"{
#include "core/beat-off.h"
};

#include <string.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <json-c/json.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <atomic>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>

#define poffsetof(type,field)((void*)(&((type*)0)->field))

static GLuint compileShader(GLenum type, const std::string &filename)
{ 
  std::ifstream in ( filename );
  std::string content ((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
  
  auto source = (const GLchar *)content.c_str();
  GLuint  shader = glCreateShader(type);
  if(!shader) return 0;
  glShaderSource(shader,1,&source,nullptr);
  glCompileShader(shader);
  GLint status;
  char log[512];
  glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
  INFO("compilation of shader %ssuccessfu",status?"was ":"was not ");
  glGetShaderInfoLog(shader,sizeof(log),NULL,log);
  INFO("compilation info log: %s\n\n",log);
  return shader;
}

template<class T>
inline void bufferData(GLenum target, const std::vector<T> &v, GLenum usage)
{
  glBufferData(target,v.size()*sizeof(T), &v[0],usage);
}
template<class T>
inline void bufferSubData(GLenum target, const std::vector<T> &v)
{
  glBufferSubData(target,0,v.size()*sizeof(T),&v[0]);
}

typedef struct vec2{
  float x;
  float y;
}vec2;
typedef struct vec4{
  float x;
  float y;
  float z;
  float w;
}vec4;

typedef struct gl_state{
  int   a_type;
  int   a_index;
  int   a_parent;
  int   a_depth;
  vec2  a_pos;
  vec2  a_size;
  vec2  a_rel_pos;
  vec4  a_color;
  vec4  a_user0;
  vec4  a_user1;
  vec4  a_user2;
  vec4  a_user3;
  static void bindAttributes(GLuint program, int divisor)
  {
    GLuint location;
    location = glGetAttribLocation(program,"a_pos");
    if(location){
      glVertexAttribPointer(location,2,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_pos));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_size");
    if(location){
      glVertexAttribPointer(location,2,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_size));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_type");
    if(location){
      glVertexAttribPointer(location,1,GL_INT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_type));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }

    location = glGetAttribLocation(program,"a_index");
    if(location){
      glVertexAttribPointer(location,1,GL_INT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_index));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_parent");
    if(location){
      glVertexAttribPointer(location,1,GL_INT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_parent));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_depth");
    if(location){
      glVertexAttribPointer(location,1,GL_INT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_depth));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_color");
    if(location){
      glVertexAttribPointer(location,4,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_color));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_rel_pos");
    if(location){
      glVertexAttribPointer(location,2,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_rel_pos));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_user0");
    if(location){
      glVertexAttribPointer(location,4,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_user0));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_user1");
    if(location){
      glVertexAttribPointer(location,4,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_user1));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_user2");
    if(location){
      glVertexAttribPointer(location,4,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_user2));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
    location = glGetAttribLocation(program,"a_user3");
    if(location){
      glVertexAttribPointer(location,4,GL_FLOAT,GL_FALSE,sizeof(gl_state),poffsetof(gl_state,a_user3));
      glEnableVertexAttribArray(location);
      glVertexAttribDivisor(location,divisor);
    }
  }
}gl_state;

typedef struct aux_state{
  int     m_index;
  size_t  m_priv_size;
  void   *m_priv_data;
  void  (*update)(struct aux_state *);
}aux_state;


typedef struct draw_type{
  std::string           m_name;
  GLuint                m_prog;
  GLuint                m_vert;
  GLuint                m_geom;
  GLuint                m_frag;
  std::vector<GLuint>   m_textures;
  draw_type(const std::string &vert = std::string()
      , const std::string &geom = std::string()
      , const std::string &frag = std::string()
      , int divisor = 1){
    m_vert = compileShader(GL_VERTEX_SHADER,vert);
    m_geom = compileShader(GL_GEOMETRY_SHADER,geom);
    m_frag = compileShader(GL_FRAGMENT_SHADER,frag);
    m_prog = glCreateProgram();

    if(m_vert)glAttachShader(m_prog,m_vert);
    if(m_geom)glAttachShader(m_prog,m_geom);
    if(m_frag)glAttachShader(m_prog,m_frag);
    glBindFragDataLocation(m_prog,0,"f_output");
    glLinkProgram(m_prog);
    glUseProgram(m_prog);
  }
  virtual void add_textures(std::initializer_list<const std::string > names)
  {
    for(auto s : names){
      GLuint  tex;
      glGenTextures(1,&tex);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,tex);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
      auto img = IMG_Load(s.c_str());
      if(img && tex){
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img->w,img->h,
            0,GL_BGRA,GL_UNSIGNED_BYTE,img->pixels);
        auto base_name = "u_"+s.substr(s.rfind('/'),s.rfind('.'));
        GLuint location = glGetUniformLocation(m_prog,base_name.c_str());
        if(location)glUniform1i(location,tex);
        location = glGetUniformLocation(m_prog,(base_name+"_size").c_str());
        if(location)glUniform2f(location, img->w,img->h);
        m_textures.push_back(tex);
      }
      SDL_FreeSurface(img);
    }
  }
  virtual void update(gl_state &gl, aux_state &aux){

  }
  virtual void draw  (std::vector<gl_state> &items){

  }
}draw_type;

typedef struct draw_registry{
  std::vector<gl_state>           m_gl_state;
  std::vector<aux_state>          m_aux_state;
  std::unordered_map<int,int>     m_index_cache;
  std::vector<draw_type>          m_types;
  std::map<int,draw_type*>        m_type_cache;
  GLuint                          m_vao;
  size_t                          m_vbo_size;
  GLuint                          m_vbo;
  virtual void update(){
    for(size_t i = 0; i < m_gl_state.size(); i++){
      if(m_type_cache.count(m_gl_state[i].a_type)){
        if(m_index_cache.count(m_gl_state[i].a_parent)){
          int parent = m_index_cache[m_gl_state[i].a_parent];
          m_gl_state[i].a_depth = m_gl_state[parent].a_depth + 1;
          m_gl_state[i].a_pos.x = m_gl_state[i].a_rel_pos.x+m_gl_state[parent].a_pos.x;
          m_gl_state[i].a_pos.y = m_gl_state[i].a_rel_pos.y+m_gl_state[parent].a_pos.y;
        }else{
          m_gl_state[i].a_depth = 0;
          m_gl_state[i].a_pos.x = m_gl_state[i].a_rel_pos.x;
          m_gl_state[i].a_pos.y = m_gl_state[i].a_rel_pos.y;
        }
        auto drawer = m_type_cache[m_gl_state[i].a_type];
        drawer->update(m_gl_state[i],m_aux_state[i]);
      }
    }
  }
  virtual void addType(int num, draw_type &&type){
    m_types.push_back(type);
    m_type_cache.insert(std::make_pair(num,&m_types.back()));
  }
  virtual void uploadData()
  {
    if(!m_vao)glGenVertexArrays(1,&m_vao);
    glBindVertexArray(m_vao);
    if(!m_vbo){
      glGenBuffers(1,&m_vbo);
      glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
      m_vbo_size = 0;
    }
    if(m_gl_state.size() * sizeof(gl_state) > m_vbo_size){
      glBufferData(GL_ARRAY_BUFFER,m_gl_state.size()*sizeof(gl_state),&m_gl_state[0],GL_STREAM_DRAW);
      m_vbo_size = m_gl_state.size()*sizeof(gl_state);
    }else{
      glBufferSubData(GL_ARRAY_BUFFER,0,m_gl_state.size()*sizeof(gl_state),&m_gl_state[0]);
    }
  }
  virtual void draw(ui_context *ctx){
    uploadData();
    glClearColor(1.f,1.f,1.f,1.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    for(auto &drawer : m_types){drawer.draw(m_gl_state);}
    SDL_GL_SwapWindow(ctx->window);
  }

}draw_registry;
