#ifndef __VAMP_H
#define __VAMP_H

#ifdef __cplusplus
extern "C" {
#endif

struct filter;

int vamp_plugin_load(struct filter * filter);
void vamp_plugin_unload(struct filter * filter);
int vamp_plugin_update(struct filter * filter, float * chunk);

#ifdef __cplusplus
}
#endif

#endif
