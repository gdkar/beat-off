#include "core/beat-off.h"

#include <atomic>
#include <thread>
#include <iostream>
struct AtomicListElem{
  std::atomic<AtomicListElem*>  next;
};
template<typename T>
struct AtomicListElemInstance:public AtomicListElem{
  const T *dptr;
  AtomicListElemInstance(T *ptr):dptr(ptr){}
};
struct AtomicListHead{
  std::atomic<AtomicListElem*>  first;
  template<typename T>
  void push_back(T *value)
  {
    auto link = new AtomicListElemInstance<T>(value);
    auto expected = first.load();
    do{
      expected = first.load();
      link->next.store(expected);
    }while(!atomic_compare_exchange_weak(first,expected, link));
  }

};
template<typename T>
struct ThreadThunkArgPack : public AtomicListElem{
  static int start_thunk(void *voidp)
  {
    ThreadThunkArgPack *pack = reinterpret_cast<ThreadThunkArgPack*>(voidp);
    return (pack->self->*(pack->funcp))(pack->argarg);
  }
  std::thread start()
  {
    return std::thread(&ThreadThunkArgPack<T>::start_thunk
        , reinterpret_cast<void*>(this));
  }
  T                  *self;
  int (T::           *funcp)(void*)  ;
  void               *argarg;
};
template<typename T>
struct ThreadRunningHook : public AtomicListElem {
  static int start_thunk(void*voidp){
    ThreadRunningHook *hook = reinterpret_cast<ThreadRunningHook*>(voidp);
    return (hook->self->*(hook->run_fn))(hook->start_arg);
  }
  virtual int start(){
    thrd = std::thread(&ThreadRunningHook::start_thunk,reinterpret_cast<void*>(this));
    return thrd.joinable()?0:-1;
  }
  virtual void stop(){(self->*stop_fn)(stop_arg);}
  virtual bool joinable()const{return thrd.joinable();}
  virtual std::thread::id get_id()const{return thrd.get_id();}
  virtual void join(){if(thrd.joinable() && std::this_thread::get_id()!=thrd.get_id())thrd.join();}
  virtual void detach(){thrd.detach();}
  ThreadRunningHook( T* _self
      , int(T::*_run_fn)(void*)
      ,void(T::*_stop_fn)(void*)
      ,void*_start_arg = nullptr
      ,void*_stop_arg  = nullptr)
    : self(_self)
    , run_fn(_run_fn)
    , stop_fn(_stop_fn)
    , start_arg(_start_arg)
    , stop_arg(_stop_arg)
    , thrd(){

    }
  virtual ~ThreadRunningHook(){
    if(joinable()){
      stop();
      join();
    }
  }
  T               *self;
  int (T::        *run_fn )(void*);
  void(T::        *stop_fn)(void*);
  void            *start_arg;
  void            *stop_arg;
  std::thread     thrd;
};
struct LibHookRegistry{
  AtomicListHead    init_hooks;
  AtomicListHead    thread_hooks;
  AtomicListHead    update_hooks;
  AtomicListHead    evtent_hooks;
  AtomicListHead    render_hooks;
  AtomicListHead    deinit_hooks;
  AtomicListHead    exit_hooks;
};

template<typename T>
struct StaticCtorDtor{
  const char *name;
  void (*ctor)(T*);
  void (*dtor)(T*);
  T *self;
  StaticCtorDtor(const char *_name, void (*_ctor)(T),void(*_dtor)(T),T *arg)
    : name(_name),ctor(_ctor),dtor(_dtor),self(arg){
    if(ctor) ctor(self);
  }

 ~StaticCtorDtor(){ if(dtor) dtor(self);}

};
template<>
struct StaticCtorDtor<void>{
  const char *name;
  void (*ctor)();
  void (*dtor)();
  StaticCtorDtor(const char *_name, void(*_ctor)(), void(*_dtor)())
    : name(_name),ctor(_ctor),dtor(_dtor){
      std::cerr << "executing the static initializer for " << name << std::endl;
      if(ctor) ctor();
    }
~StaticCtorDtor(){
  
      std::cerr << "executing the static deestructor for " << name << std::endl;
  if(dtor)dtor();}

};
