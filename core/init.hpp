#ifndef CORE_INIT_H
#define CORE_INIT_H

#ifdef __cplusplus

typedef void (*ctor_fn)();
typedef void (*dtor_fn)();
class BeatOffStaticInit{
    ctor_fn     m_ctor;
    ctor_fn     m_dtor;
    const char *m_name;
  public:
    BeatOffStaticInit(const char *name, ctor_fn ctor, dtor_fn dtor)
      : m_name(name), m_ctor(ctor), m_dtor(dtor){ if(m_ctor)(*m_ctor)();}
  ~BeatOffStaticInit(){if(m_dtor)(*m_dtor)();}
  private:
//  BeatOffStaticInit(){}
//  BeatOffStaticInit(const BeatOffStaticInit &)            = delete;
//  BeatOffStaticInit &operator=(const BeatOffStaticInit &) = delete;
};
#define BEAT_OFF_REGISTER_MODULE(module, c, d)                       \
  static void beat_off_static_ctor_for_ ## module (){ c; }           \
  static void beat_off_static_dtor_for_ ## module (){ d; }           \
  BeatOffStaticInit beat_off_static_init_for_ ## module ( #module,   \
      beat_off_static_ctor_for_ ## module,                           \
      beat_off_static_dtor_for_ ## module );

#endif

#endif
