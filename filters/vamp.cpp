#include "filters/vamp.h"

#include <vamp-hostsdk/PluginHostAdapter.h>
#include <vamp-hostsdk/PluginInputDomainAdapter.h>
#include <vamp-hostsdk/PluginLoader.h>
#include "core/debug.h"
extern "C" {
#include "core/audio.h"
#include "filters/filter.h"
}

using namespace std;

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

int vamp_plugin_load(struct filter * filter){
    Plugin *plugin;
    Plugin::OutputDescriptor od;
    PluginLoader *loader = PluginLoader::getInstance();
    PluginLoader::PluginKey key;
    RealTime rt;

    // Set existing vamp plugin to NULL
    filter->vamp_plugin = 0;

    key = loader->composePluginKey(string(filter->vamp_so), string(filter->vamp_id));
    plugin = loader->loadPlugin(key, 96000, PluginLoader::ADAPT_ALL);
    //plugin = loader->loadPlugin(key, SAMPLE_RATE, 0);

    if(!plugin){
        ERROR("Unable to load vamp plugin %s\n", filter->vamp_so);
        return 1;
    }
    INFO("Loaded vamp plugin: %s\n", plugin->getIdentifier().c_str());

    Plugin::OutputList outputs = plugin->getOutputDescriptors();
    if(outputs.size() <= filter->vamp_output){
        ERROR("vamp plugin: no output %d\n", filter->vamp_output);
        return 1;
    }
    //od = outputs[filter->vamp_output];
    // TODO Do something with:
    // filter->output.label     od.identifier.c_str()

    if(!plugin->initialise(1, 256 , 256)){
        ERROR("initing vamp plugin\n");
        return 1;
    }
    if(plugin->getInputDomain() != Plugin::TimeDomain){
        ERROR("Input not in time domain\n");
        return 1;
    }

    filter->vamp_plugin = (vamp_plugin_p) plugin;
    
    return 0;
}

static long rt_msec(RealTime rt){
    return rt.sec * 1000 + rt.msec();
}

int vamp_plugin_update(struct filter * filter, float *chunk){
    // Returns number of events processed 
    int n_filtered_chunks  = 0;   
    Plugin * plugin = (Plugin *) filter->vamp_plugin;
    RealTime rt = RealTime::frame2RealTime(n_filtered_chunks*256, 96000);
    int n_features = 0;
    int event_time;
    double event_value;

    if(!plugin)
        return 0;

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
        
//        filter->update(filter, event_time, event_value);
        n_features++;
    }

    return n_features;
}

void vamp_plugin_unload(struct filter* filter){
    Plugin * plugin = (Plugin *) filter->vamp_plugin;
    filter->vamp_plugin = 0;
    delete plugin;
}
