#include "render/renderable.h"


static void _bind_attribute(GLuint prog, const char *where, int count, GLenum type, GLsizei stride, void *offset, int divisor)
{
  GLuint location = glGetAttribLocation(prog,where);
  if(location){
    glVertexAttribPointer(location,count,type,stride,offset);
    glEnableVertexAttribArray(location);
    glVertexAttribDivisor(location,divisor);
  }
}
static ssize_t gl_type_size(GLenum type){
  switch (type){
    case GL_DOUBLE:return 8;
    case GL_INT:
    case GL_FLOAT:
    case GL_UNSIGNED_INT:  return 4;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:return 2;
    case GL_BYTE:
    case GL_UNSIGNED_BYTE: return 1;
    defult:return -1;
  }
}
#define bind_attribute(prog,obj,field, type,divisor)\
  _bind_attribute(prog,STRINGIFY(field),sizeof(obj.field)/gl_type_size(type),type,sizeof(obj),poffsetof(obj,field),divisor)

static void bind_gl_state_attributes(GLuint prog)
{
  bind_attribute(prog,gl_state, a_pos,   GL_FLOAT,1);
  bind_attribute(prog,gl_state, a_size,  GL_FLOAT,1);
  bind_attribute(prog,gl_state, a_type,  GL_INT,  1);
  bind_attribute(prog,gl_state, a_index, GL_INT,  1);
  bind_attribute(prog,gl_state, a_parent,GL_INT,  1);
  bind_attribute(prog,gl_state, a_depth, GL_INT,  1);
  bind_attribute(prog,gl_state, a_color, GL_FLOAT,1);
  bind_attribute(prog,gl_state, a_user0, GL_FLOAT,1);
}

renderable *renderable_new(struct json_object *config)
{
  renderable *r = calloc(sizeof(renderable),1);
  r->config     = config;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
            // create window and GL context
  r->window = SDL_CreateWindow("opengl", 100, 100, 640, 480,SDL_WINDOW_OPENGL);
  r->context = SDL_GL_CreateContext(window);
            // initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();
  glGetError(); // ignore GL_INVALID_ENUM after glewInit(), see
                          // http://www.opengl.org/wiki/OpenGL_Loading_Library
            // some GL settings
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1*r->vao);
  glBindVertexArray(r->vao);
  glGenBuffers(1,&r->vbo);
  glBindBuffer(GL_ARRAY_BUFFER,r->vbo);
  return r;
}

void renderable_del(renderable *r)
{
  for(int i = 0; i < r->nb_items;i++){
    int type = r->items[i].a_type;
    if(r->renderers[type]->del_instance){
      r->renderers[type]->del_instance(r->renderers[type], &r->items[i],&r->items_aux[i]);
    }
  }
  free(r->items);
  free(r->items_aux);
  for(int i = 0; i < r->nb_renderers;i++){
    if(r->renderers[i]->cleanup)
      r->renderers[i]->cleanup(r->rendereres[i]);
      render_type *re = r->renderers[i];
      if(re->program)glDeleteProgram(re->program);
      re->program=0;
      if(re->nb_shaders){
        for(int j = 0; j < re->nb_shaders; j++){
          glDeleteShader(re->shaders[j]);
          re->shaders[i] = 0;
        }
        re->nb_shaders =0;
        free(re->shaders);
      }
      if(re->nb_textures){
        for(int j = 0; j < re->nb_textures;j++){
          glDeleteTexture(re->textures[j];
          re->textures[i]=0;
        }
      }re->nb_textures=0;
      free(re->textures);
      free(re);
      r->renderers[i] = NULL;
  }
  r->nb_renderers = 0;
  free(r->renderers);
  glDeleteVertexArray(r->vao);r->vao=0;
  glDeleteBuffer(r->vbo);r->vbo=0;
  glDeleteFrameBuffers(2,r->fbo);
  r->fbo[0]=r->fbo[1]=0;
  r->nb_items_buffered = 0;
  SDL_GL_DeleteContext(r-context);
  SDL_DestroyWindow(r->window);
  r->window = NULL;
  json_object_put(r->config);
  free(r);
}
int renderable_add_renderer(renderable *r,render_type *type)
{
  for(int i = 0; i < r->nb_renderers;i++){
    if(r->renderers[i]==r)return i;
  }
  r->renderers=realloc(r->renderers,sizeof(render_type*)*(r->nb_renderers+1));
  type->typecode = r->nb_renderers;
  r->nb_renderers++;
  if(type->initialize){
    type->initialize(type);
  }
  return type->typecode;
}
int renderable_add_item(renderable *r, int type)
{
  render_Type *t=NULL;
  if(!r->renderers[type]->typecode==type){
    for(int i = 0; i <  r->nb-renderers;i++){
        if(r->renderers[i]->typecode == type){
          t=r->renderers[i];break;
        }
    }
  }else{
    t=r->renderers[type];
  }
}
