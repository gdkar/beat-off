#include <math.h>

#include "util/siggen.h"
#include "util/math.h"
#include "core/parameter.h"

quant_labels_t osc_quant_labels = {
    [OSC_SINE] = "Sine",
    [OSC_TRIANGLE] = "Triangle",
    [OSC_SQUARE] = "Square",
    [OSC_SAWTOOTH_R] = "Sawtooth Rising",
    [OSC_SAWTOOTH_F] = "Sawtooth Falling",
    LABELS_END
};

float osc_fn_gen(enum osc_type type, float phase){
    switch(type){
        case OSC_SINE:
        default:
            return (cosf(phase * 2 * (float)M_PI) + 1.f) *0.5;
        case OSC_TRIANGLE:
            return fabsf(fmodf(phase + 16.f, 1.f) - 0.5f) * 2;
        case OSC_SAWTOOTH_R:
            return fmodf(phase + 16.f, 1.f);
        case OSC_SAWTOOTH_F:
            return fmodf(-phase + 16.f, 1.f);
        case OSC_SQUARE:
            return (fmodf(phase + 16.0f + 0.75f, 1.f) > 0.5f ? 1.f : 0.f);
    }
}

void osc_quantize_parameter_label(float val, char * buf, int n){
    int v = quantize_parameter(osc_quant_labels, val);
    strncpy(buf, osc_quant_labels[v], n);
}

