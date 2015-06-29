#ifndef __UI_WAVEFORM_H__
#define __UI_WAVEFORM_H__

#include "waveform/waveform.h"
#include <SDL/SDL.h>

void ui_waveform_render(SDL_Surface *onto, rect_t *where);
void ui_waveform_init();
void ui_waveform_del();

#endif
