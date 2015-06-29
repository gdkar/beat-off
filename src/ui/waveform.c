#include "core/beat-off.h"
#include "util/math.h"
#include "ui/waveform.h"
#include "ui/layout.h"
#include "core/err.h"
#include "waveform/waveform.h"

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

SDL_Surface* waveform_surface;
int skip = 1;

void ui_waveform_init(){
    if(layout.waveform.skip * layout.waveform.w < WAVEFORM_HISTORY_SIZE)
        skip = layout.waveform.skip;
}

void ui_waveform_render(SDL_Surface *onto, rect_t *where){
    rect_t r;
    rect_origin(&layout.waveform.rect, &r);
    r.x+=where->x;r.y+=where->y;
    SDL_FillRect(onto, where, SDL_MapRGB(onto->format, 30, 30, 30));
    int h;
    SDL_Color c;
    if(skip < 1) skip = 1;
    for(int j = 0; j < N_WF_BINS; j++){
        float * history = waveform_bins[j].history;
        int hptr = waveform_bins[j].hptr; 
        hptr -= hptr % skip;
        c = waveform_bins[j].color;
        for(int i = 0; i < layout.waveform.w; i++)
        {
            float x = 0.;
            for(int k = 0; k < skip; k++){
                hptr--;
                if(hptr < 0)
                    hptr = WAVEFORM_HISTORY_SIZE;
                x = MAX(x, history[hptr]);
            }
            x = MIN(x,1.0);
            h = x * layout.waveform.h;
            vlineRGBA(onto, where->x+layout.waveform.w - i, where->y+(layout.waveform.h + h) / 2, where->y+(layout.waveform.h - h) / 2, c.r, c.g, c.b, 255);
        }
    }
    for(int i = 0; i < layout.waveform.w; i++){
        char l = 0;
        for(int k = 0; k < skip; k++){
            l = l | beat_lines[i * skip + k];
        }
        if(l & 1)
            vlineRGBA(onto, layout.waveform.w - i + where->x, layout.waveform.y+where->y,where->y+ layout.waveform.h, 128, 255, 0, 200);
        if(l & 2)
            vlineRGBA(onto, layout.waveform.w - i + where->x, layout.waveform.y+where->y,where->y+ layout.waveform.h, 255, 128, 0, 200);
        if(l & 4)
            vlineRGBA(onto, layout.waveform.w - i + where->x, layout.waveform.y+where->y, where->y+layout.waveform.h, 255, 0, 0, 200);
         
    }
}

void ui_waveform_del(){
}
