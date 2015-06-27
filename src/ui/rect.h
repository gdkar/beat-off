#pragma once

#include "ui/item.h"
#include "ui/vertexattrib.h"
#include "core/beat-off.h"
#include <fstream>
#include <SDL2/SDL.h>
#include <iostream>
struct RectangleGroup{
  VertexAttrib<float,4> m_bg_color;
  VertexAttrib<float,4> m_fg_color;
  VertexAttrib<float,2> m_border_width;
  GLuint                m_program;
  vector<GLuint>        m_shaders;
  SDL_Window           *m_window;
  SDL_GLContext         m_context;
  explicit RectangleGroup()
  : m_bg_color("a_bg_color")
    , m_fg_color("a_fg_color")
    , m_border_width("a_border"){
  }
  RectangleGroup &operator =(const RectangleGroup &other) = default;
  RectangleGroup &operator =(RectangleGroup &&other) = default;
  explicit RectangleGroup( std::initializer_list<std::pair<int,string>>shader_names)
    : m_bg_color("a_bg_color")
    , m_fg_color("a_fg_color")
    , m_border_width("a_border")
  {
    std::cerr << "initializing glfw"<<std::endl;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);

    // create window and GL context
    m_window = SDL_CreateWindow("opengl", 100, 100, 800, 600,SDL_WINDOW_OPENGL);
    m_context = SDL_GL_CreateContext(m_window);
    // initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); 

    // some GL settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glewExperimental=GL_TRUE;
    glewInit();
    std::cerr << "creating glfw window"<<std::endl;
    m_context= glfwCreateWindow(0,0,"window",nullptr,nullptr);
    std::cerr << "initializing glew"<<std::endl;
    std::cerr<<"creating program"<<std::endl;
    m_program = glCreateProgram();
    for(auto desc : shader_names)
    {
      std::cerr << "opening file onto "<<desc.second<<std::endl;
      std::ifstream in(string(desc.second));
      std::cerr << "reading contents"<<std::endl;
      std::string contents((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
      std::cerr<<"file contains: "<<contents<<std::endl<<std::endl;
      auto source = (const GLchar*) contents.c_str();
      GLuint shader = glCreateShader(desc.first);
      glShaderSource(shader,1,&source,nullptr);
      glCompileShader(shader);
      m_shaders.push_back(shader);
      GLint status;
      glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
      INFO("compilation of shader was %ssuccessful",status?"":"not ");
      char log[512];
      glGetShaderInfoLog(shader,sizeof(log),NULL,log);
      INFO("%s",log);
      glAttachShader(m_program, shader);
    }
    glBindFragDataLocation(m_program,0,"f_color");
    glLinkProgram(m_program);
    GLint status;
    char log[512];
    glGetProgramiv(m_program,GL_LINK_STATUS,&status);
    std::cerr << "link status  returned "<<status<<std::endl;
    glGetProgramInfoLog(m_program,sizeof(log),NULL,log);
    INFO("%s",log);
    m_bg_color.bindAttribute(m_program);
    glGetProgramiv(m_program,GL_VALIDATE_STATUS,&status);
    std::cerr << "validate returned "<<status<<std::endl;

    glGetProgramInfoLog(m_program,sizeof(log),NULL,log);
    INFO("%s",log);
    m_fg_color.bindAttribute(m_program);
    glGetProgramInfoLog(m_program,sizeof(log),NULL,log);
    INFO("%s",log);

    m_border_width.bindAttribute(m_program);
    glGetProgramInfoLog(m_program,sizeof(log),NULL,log);
    INFO("%s",log);
  }
  void alsoBind(PositionGroup &pgr){
    pgr.m_position.bindAttribute(m_program);
    pgr.m_size.bindAttribute(m_program);
    pgr.m_depth.bindAttribute(m_program);
    pgr.m_id.bindAttribute(m_program);
  }
};

extern RectangleGroup rectangle_group;
