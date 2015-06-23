#include "filters/vamp.h"

#include <vamp-hostsdk/PluginHostAdapter.h>
#include <vamp-hostsdk/PluginInputDomainAdapter.h>
#include <vamp-hostsdk/PluginLoader.h>

extern "C" {
#include "audio/audio.h"
#include "core/config.h"
#include "filters/filter.h"
}

using namespace std;

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

static unsigned long vamp_chunk_size = 128;

int vamp_plugin_load(filter_t * filter){
    Plugin *plugin;
    Plugin::OutputDescriptor od;
    PluginLoader *loader = PluginLoader::getInstance();
    PluginLoader::PluginKey key;
    RealTime rt;

    // Set existing vamp plugin to NULL
    filter->vamp_plugin = 0;

    key = loader->composePluginKey(string(filter->vamp_so), string(filter->vamp_id));
    plugin = loader->loadPlugin(key, config.audio.sample_rate, PluginLoader::ADAPT_ALL);
    //plugin = loader->loadPlugin(key, SAMPLE_RATE, 0);

    if(!plugin){
        printf("Unable to load vamp plugin %s\n", filter->vamp_so);
        return 1;
    }
    printf("Loaded vamp plugin: %s\n", plugin->getIdentifier().c_str());

    Plugin::OutputList outputs = plugin->getOutputDescriptors();
    if(outputs.size() <= filter->vamp_output){
        printf("Error with vamp plugin: no output %d\n", filter->vamp_output);
        return 1;
    }
    //od = outputs[filter->vamp_output];
    // TODO Do something with:
    // filter->output.label     od.identifier.c_str()

    vamp_chunk_size = config.audio.chunk_size;
    if(!plugin->initialise(1, vamp_chunk_size, vamp_chunk_size)){
        printf("Error initing vamp plugin\n");
        return 1;
    }
    if(plugin->getInputDomain() != Plugin::TimeDomain){
        printf("Input not in time domain\n");
        return 1;
    }

    filter->vamp_plugin = (vamp_plugin_p) plugin;
    
    return 0;
}

static long rt_msec(RealTime rt){return rt.sec * 1000 + rt.msec();}

int vamp_plugin_update(filter_t * filter, chunk_pt chunk){
    // Returns number of events processed 
    Plugin * plugin = (Plugin *) filter->vamp_plugin;
    int n_features = 0;
    int event_time;
    double event_value;
    if(!plugin) return 0;
    RealTime rt = RealTime::frame2RealTime(n_filtered_chunks*config.audio.chunk_size, config.audio.sample_rate);
    Plugin::FeatureSet features = plugin->process(&chunk, rt);
    for(vector<Plugin::Feature>::iterator it =features[filter->vamp_output].begin(); it != features[filter->vamp_output].end(); it++){
        if(it->hasTimestamp)
            event_time = rt_msec(it->timestamp);
        else
            event_time = rt_msec(rt);
        if(!it->values.empty())
            event_value = it->values.front();
        else
            event_value = 0.0;
        filter->update(filter, event_time, event_value);
        n_features++;
    }
    return n_features;
}
void vamp_plugin_unload(filter_t * filter){
    Plugin * plugin = (Plugin *) filter->vamp_plugin;
    filter->vamp_plugin = 0;
    delete plugin;
}
