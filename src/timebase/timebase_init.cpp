extern "C"{
#include "timebase/timebase.h"
#include "waveform/waveform.h"
};
#include "entity/entity.h"

static StaticCtorDtor<void> timebase_ctor_dtor  = StaticCtorDtor<void>( "timebase constructor/destructor",&timebase_init, &timebase_del );

static StaticCtorDtor<void> waveform_ctor_dtor  = StaticCtorDtor<void>( "waveform constructor / destructor", &waveform_init, &waveform_del);
