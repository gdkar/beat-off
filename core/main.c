#include "core/internal.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL2_framerate.h>
int main()
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    //return 0;
    //load_layout(&layout, "layout.ini");
    FPSmanager fps_manager;

    SDL_initFramerate(&fps_manager);
    SDL_setFramerate(&fps_manager, 40);
    SDL_Event received;

//    widget *box1    = widget_create(&hbox_class, mainwin,"Box1",32., 32., 128,128,128,192,192,192,160,360,320,120);
    
//    (void)box1;
    object * window = object_create ( &window_class, NULL, "Main Window", 640, 480, 128, 128, 128, 255);
    object_set_root(window);
    while(1){
      while(SDL_WaitEvent(&received)){
        object_event_bubble  ( &received );
      }
        //float tb = (float)timebase_get() / 1000; // TODO make all times long
        /*
        mbeat_t tb = timebase_get();

        update_patterns(tb);
        update_hits(tb);
        update_signals(tb);
        */


        double stat_fps = 1000. / SDL_framerateDelay(&fps_manager);
        (void)stat_fps;
    }
    object_delete(window);
    return 0;
}

