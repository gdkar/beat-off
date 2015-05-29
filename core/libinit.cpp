#include "core/internal.h"
#include <SDL2/SDL_audio.h>
namespace {
  class BeatOffConstructorDestructor {
    public:
    BeatOffConstructorDestructor( int _mandatory) {
      UNUSED(_mandatory);
      INFO("starting up.");
      SDL_Init(SDL_INIT_EVERYTHING);
    }
   ~BeatOffConstructorDestructor(){
     INFO("shutting down");
      SDL_Quit();
   }
  };
BeatOffConstructorDestructor  beat_off_ctor_dtor_instance{0};
};

