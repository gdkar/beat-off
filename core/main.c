#include "core/internal.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL2_framerate.h>

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    //return 0;
    //load_layout(&layout, "layout.ini");

    FPSmanager fps_manager;

    SDL_initFramerate(&fps_manager);
    SDL_setFramerate(&fps_manager, 40);
    SDL_Event received;
    while(1){
      while(SDL_PollEvent(&received)){
      DEBUG("event  of type %d",received.type);
      }
        //float tb = (float)timebase_get() / 1000; // TODO make all times long
        /*
        mbeat_t tb = timebase_get();

        update_patterns(tb);
        update_hits(tb);
        update_signals(tb);
        */


        double stat_fps = 1000. / SDL_framerateDelay(&fps_manager);
        DEBUG("stat_fps %F",stat_fps);;
    }
    SDL_Quit();
    return 0;
}

