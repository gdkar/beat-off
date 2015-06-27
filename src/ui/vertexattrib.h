#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <memory>
#include <atomic>
#include <map>
#include <thread>
#include <array>
#include "core/err.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

template<typename T, int N>
struct  VertexAttrib : public vector<array<T,N> >{
  static GLenum data_type(const std::type_info &info){
    if(info.hash_code()==typeid(int).hash_code())                return GL_INT;
    else if(info.hash_code()==typeid(unsigned int).hash_code())  return GL_UNSIGNED_INT;
    else if(info.hash_code()==typeid(short).hash_code())         return GL_SHORT;
    else if(info.hash_code()==typeid(unsigned short).hash_code())return GL_UNSIGNED_SHORT;
    else if(info.hash_code()==typeid(char).hash_code())          return GL_BYTE;
    else if(info.hash_code()==typeid(unsigned char).hash_code()) return GL_UNSIGNED_BYTE;
    else if(info.hash_code()==typeid(float).hash_code())         return GL_FLOAT;
    else if(info.hash_code()==typeid(double).hash_code())        return GL_DOUBLE;
    else ERROR("Invalid type for OpenGL attributes\n");
    return -1;
  }
  string                  m_name;
  int                     m_divisor = 0;
  GLboolean               m_normalized = false; 
  GLenum                  m_data_type;
  GLuint                  m_vbo = 0;
  GLuint                  m_vao = 0;
  GLuint                  m_prog = 0;
  VertexAttrib &operator = (VertexAttrib &&other)=default;
  VertexAttrib &operator = (const VertexAttrib &other)=default;
  VertexAttrib(VertexAttrib &&other) = default;
  VertexAttrib(const string &name, int _divisor = 0, bool _normalized=false)
  : m_name(name)
  , m_divisor(_divisor)
  , m_normalized(_normalized)
  , m_data_type(data_type(typeid(T))){}
  GLenum           data_type()const{return m_data_type;}
  GLboolean        normalized()const{return m_normalized;}
  GLuint           components()const{return N;}
  GLuint           item_size ()const{return N*sizeof(T);}
  GLuint           divisor   ()const{return m_divisor;}
  bool  bindAttribute(GLuint program)
  {
    m_prog = program;
    if(!m_vao) glGenVertexArrays(1,&m_vao);
    if(!m_vao) return false;
    glBindVertexArray(m_vao);
    if(!m_vbo) glGenBuffers(1,&m_vbo);
    if(!m_vbo) return false;
    glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vector<array<T,N>>::size()*sizeof(array<T,N>),vector<array<T,N> >::data(),GL_STREAM_DRAW);
    GLuint location = glGetAttribLocation(program, m_name.c_str());
    if(!location) return false;
    glVertexAttribPointer(location, components(), data_type(), normalized(),item_size(),(void*)0);
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,divisor());
    return true;
  }
  ~VertexAttrib(){
    if(m_vao) glDeleteVertexArrays(1,&m_vao);
    m_vao = 0;
    if(m_vbo) glDeleteBuffers(1,&m_vbo);
    m_vbo = 0;
  }
};



