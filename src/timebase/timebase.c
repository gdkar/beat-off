#include "timebase/timebase.h"

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include "waveform/waveform.h"
#include "audio/audio.h"
#include "core/config.h"
#include "core/err.h"

int stat_fps = 0;
int stat_ops = 0;

static double ui_bpm = 140.;
static double freq_mb_per_ms = 140. / 60;
static long pt_ms = 0;
static long pt_mb = 0;

static SDL_mutex* updating;
volatile int timebase_source = TB_AUTOMATIC;

void timebase_init()
{
    updating = SDL_CreateMutex();
    if(!updating) FAIL("Unable to create mutex: %s\n", SDL_GetError());

    pt_ms = 0;
    pt_mb = 0;
    freq_mb_per_ms = 140. / 60;
    ui_bpm = 140.;
}

void timebase_del()
{
    SDL_DestroyMutex(updating);
}

/* DELETEME
void timebase_update(chunk_pt chunk)
{
}
*/

static long get_cur_mb(long cur_ms)
{
    return pt_mb + (long)((double)(cur_ms - pt_ms) * freq_mb_per_ms);
}

void timebase_tap(double alpha)
{
    long cur_ms = SDL_GetTicks();

    long cur_mb = get_cur_mb(cur_ms);

    long error_mb = ((cur_mb + 1500) % 1000) - 500;

    if(SDL_LockMutex(updating)) FAIL("Unable to lock mutex: %s\n", SDL_GetError());

    double new_freq = (1000. - (double)error_mb) / (double)(cur_ms - pt_ms);
    freq_mb_per_ms = alpha * new_freq + (1. - alpha) * freq_mb_per_ms;

    pt_ms = cur_ms;
    pt_mb = cur_mb;

    ui_bpm = ui_bpm * config.timebase.ui_bpm_alpha + (1.0 - config.timebase.ui_bpm_alpha) * (freq_mb_per_ms * 60.);

    if(SDL_UnlockMutex(updating)) FAIL("Unable to unlock mutex: %s\n", SDL_GetError());
}

long timebase_get()
{
    long cur_ms = SDL_GetTicks();
    static int last_result = 0;

    if(SDL_LockMutex(updating)) FAIL("Unable to lock mutex: %s\n", SDL_GetError());
    long result = get_cur_mb(cur_ms);
    if(SDL_UnlockMutex(updating)) FAIL("Unable to unlock mutex: %s\n", SDL_GetError());

    if((result % 1000) < (last_result % 1000)){
        beat_lines[0] |= 4;
    }

    last_result = result;

    return result - config.timebase.antidelay_ms;
}

float timebase_get_bpm()
{
    //return freq_mb_per_ms * 60;
    return ui_bpm;
}

void timebase_align(){
    pt_mb += 1000;
}

// (mb/ms) * (1000 b / mb) * (s / 1000 ms) * (60 m /s) = 60 (mb / ms) / bpm
