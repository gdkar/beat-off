extern "C"{
#include "core/beat-off.h"
#include "entity/entity.h"
#include "entity/window.h"
#include <stdatomic.h>
}
#include <unordered_map>
#include <vector>


struct attribute : public renderable{
/*  struct hash_record {
    entity_id       id;
    uint32_t        index;
  };
  struct dense_record{
    entity_id       id;
    uint32_t        hash;
  };*/
  const size_t                                m_item_size;
  size_t                                m_item_count;
  std::unordered_map<uint32_t, hash_record> m_hash;
  DenseBase                            *m_data;
  attribute(size_t size):m_item_size(size),
  m_data(0){}

  
  dense_record *at(entity_id ent)
  {
    hash_record &hrec = m_hash.at((uint32_t)hashfn(ent));
    return reinterpret_cast<dense_record*>(&m_data[m_item_size * hrec.index]);
  }
  bool insert(dense_record *drec)
  {
    drec->hash = (uint32_t)hashfn(drec->id);
    if(m_hash.count(drec->hash))
    {
      hash_record &hrec = m_hash.at(drec->hash);
      m_data->put(hrec.index,drec);
      return true;
    }else{
      hash_record hrec = (hash_record){drec->id,m_hash.size()};
      m_hash.insert(std::make_pair(drec->hash,hrec));
      m_data->push(drec);
      return true;
    }
    return false;
  }
  bool erase(entity_id ent)
  {
    uint32_t hash_val = (uint32_t)hashfn(ent);
    if(!m_hash.count(hash_val)) return false;
    hash_record &hrec = m_hash.at(hash_val);
    m_data->swap(hrec.index,m_data->size()-1);
    m_data->pop();
    auto drec = reinterpret_cast<DenseBase::DenseRecord*>(m_data->get(hrec.index));
    m_hash.at(drec->hash).index = hrec.index;
    m_hash.erase(hash_val);
    return true;
  } 
  template<typename T>
  static attribute MakeAttribute(){ return attribute(sizeof(T),new DenseVector<sizeof(T)>);}
private:
  attribute(size_t size, DenseBase *data):m_item_size(size),m_data(data){}
};

extern "C"{
  attribute *attribute_alloc_(size_t item_size){
    return new attribute(item_size);
  }
  void attribute_free(attribute *att){
    delete att;
  }
  dense_record *attribute_lookup(attribute *att, entity_id ent)
  {
    return att->at(ent);
  }
  bool attribute_insert(attribute *att, dense_record *rec)
  {
    return att->insert(rec);
  }
  bool attribute_erase(attribute *att, entity_id ent)
  {
    return att->erase(ent);
  }
}
void rect_geometry_render_fn(window_ctx *ctx)
{
  
}
attribute rect_geom_attr{sizeof(rect_geometry)};

struct Window : public window_ctx{
  private:
    typedef std::pair<Window*,int (Window::*)()>  arg_pack;
    static void resize_callback(GLFWwindow*window,int width,int height)
    {
      glfwMakeContextCurrent(window);
      glViewport(0,0,width,height);
    }
  public:
  Window(){
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    glfwInit();
    
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);

    width  = 640;
    height = 480;

    window = glfwCreateWindow(width,height,"Window",monitor,NULL);
      if(!window)
      {
        glfwTerminate();
        FAIL("failed to create GLFW window\n");
      }
      {
        auto args = new arg_pack;
        *args = std::make_pair(this,&Window::events_fn);
        thrd_create(&events_thread, &Window::pmf_thunk,args);
      }
    }
    ~Window(){
        glfwSetWindowShouldClose(window,GL_TRUE);
        thrd_join(events_thread,NULL);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    int events_fn ( )
    {
      while(!glfwWindowShouldClose(window))
      {
        glfwMakeContextCurrent(window);
        renderable *to_render = atomic_load(&renderable_list);
        while(to_render ){
          if(to_render->render_fn){
            to_render->render_fn(this);
          }
          to_render = atomic_load(&to_render->next);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
        glClearColor(0,0,0,1.0);
        glClear(GL_COLOR_BUFFER_BIT);
      }
      return 0;
    }
  private:
    static int pmf_thunk(void *voidp)
    {
      arg_pack *args = reinterpret_cast<arg_pack*>(voidp);
      Window *first = args->first;
      int(Window::*second)() = args->second;
      delete args;
      return (first->*second)();
    }
};

Window win_ctx;
