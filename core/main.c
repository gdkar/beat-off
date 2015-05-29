#include "core/internal.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL2_framerate.h>
#include "ui/mainwin.h"
int main()
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    SDL_Init(SDL_INIT_EVERYTHING);
    //return 0;
    //load_layout(&layout, "layout.ini");
    FPSmanager fps_manager;

    SDL_initFramerate(&fps_manager);
    SDL_setFramerate(&fps_manager, 40);
    SDL_Event received;
    widget *mainwin = widget_create(&mainwin_class,NULL,"The Main Window");
    struct xy pt;
    while(1){
      while(SDL_WaitEvent(&received)){
        switch(received.type){
          case SDL_MOUSEMOTION:pt = (struct xy){received.motion.x,received.motion.y};break;
          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP:pt = (struct xy){received.button.x,received.button.y};break;
          case SDL_FINGERDOWN:
          case SDL_FINGERUP:
          case SDL_FINGERMOTION:pt = (struct xy){received.tfinger.x,received.tfinger.y};break;
          default:
            pt.x=-1,pt.y=-1;
        };
        widget_event_filter(mainwin,&pt, &received,NULL);
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
        (void)stat_fps;
    }
    SDL_Quit();
    return 0;
}

