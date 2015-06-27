#include <portmidi.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>

#include "core/err.h"
#include "core/config.h"
#include "midi/midi.h"
#include "midi/config.h"
#include "core/parameter.h"
#include "ui/ui.h"
#include "util/math.h"

#define MIDI_BUFFER_SIZE 256

static int midi_running;
static SDL_Thread* midi_thread;

static PmEvent events[MIDI_BUFFER_SIZE];

SDL_Color midi_handle_color = {150, 150, 150, 255};

struct midi_controller * midi_controllers;
int n_midi_controllers;

PmError pm_errmsg(PmError err){
    printf("CAUGHT PM ERROR\n");
    return err;
}

int midi_refresh_devices(){
    PmError err;
    int n = Pm_CountDevices();

    struct midi_controller * new_controllers = malloc(n * sizeof(struct midi_controller));
    if(!new_controllers) FAIL("Unable to malloc for MIDI controllers.\n");
    memset(new_controllers, 0, n * sizeof(struct midi_controller));
    int n_new = 0;

    // Check for available devices
    for(PmDeviceID i = 0; i < n; i++) {
        const PmDeviceInfo* device = Pm_GetDeviceInfo(i);
        if(!device){
            ERROR("Unable to get device info for PM device %d\n", i);
            goto refresh_fail;
        }
        if(device->input) {
            new_controllers[n_new].device_id = i;
            new_controllers[n_new].name = device->name;
            n_new++;
        }
    }
    // Read configuration from file, populating `.enabled` if found
    if(midi_config_load(config.path.midi, new_controllers, n_new)){
        ERROR("Unable to read MIDI configuration file %s\n", config.path.midi);
        goto refresh_fail;
    }

    // Delete old midi controllers
    if(midi_controllers){
        for(int i = 0; i < n_midi_controllers; i++){
            if(midi_controllers[i].stream){
                err = Pm_Close(midi_controllers[i].stream);
                if(err != pmNoError) 
                    WARN("Could not close MIDI device: %s (%s); %s", 
                            midi_controllers[i].short_name, 
                            midi_controllers[i].name, 
                            Pm_GetErrorText(err));
            }
        }
        free(midi_controllers);
    }

    midi_controllers = new_controllers;
    n_midi_controllers = n_new;

    // Open corresponding PortMidiStream's
    for(int i = 0; i < n_new; i++) {
        if(new_controllers[i].enabled){
            err = Pm_OpenInput(&new_controllers[i].stream, new_controllers[i].device_id, NULL, MIDI_BUFFER_SIZE, NULL, NULL);
            if(err != pmNoError){
                ERROR("Could not open MIDI device: %s\n", Pm_GetErrorText(err));
                new_controllers[i].device_id = 0;
                new_controllers[i].enabled = 0;
                new_controllers[i].stream= NULL;
                //goto refresh_fail; // Soft fail
            }else{
                printf("Connected MIDI device: %s (%s)", new_controllers[i].short_name, new_controllers[i].name);
            }
        }
    }


    return 0;

refresh_fail:
    if(new_controllers){
        for(int i = 0; i < n_new; i++){
            if(new_controllers[i].stream){
                err = Pm_Close(new_controllers[i].stream);
                if(err != pmNoError) 
                    WARN("Could not close MIDI device: %s (%s); %s", 
                            new_controllers[i].short_name, 
                            new_controllers[i].name, 
                            Pm_GetErrorText(err));
            }
        }
        free(new_controllers);
    }
    return -1;
}

static int midi_check_errors(struct midi_controller * controller){
    if(!controller->stream) return 0;
    PmError err = Pm_HasHostError(controller->stream);
    if(err < 0){
        WARN("MIDI Host error: %s\n", Pm_GetErrorText(err));

        // Close stream  & disconnect 
        Pm_Close(controller->stream);
        controller->stream = 0;

        // Refresh devices (attempt to reconnect the device that we just lost) 
        // TODO
        //midi_refresh_devices();
        return 1;
    }
    return 0;
}

static int midi_run(void* args)
{
    PmError err;

    err = Pm_Initialize();
    if(err != pmNoError) FAIL("Could not initialize PortMIDI: %s\n", Pm_GetErrorText(err));

    midi_refresh_devices();

    while(midi_running)
    {
        for(int i = 0; i < n_midi_controllers; i++)
        {
            struct midi_controller * controller = &midi_controllers[i];
            if(!controller->stream) continue;
            if(midi_check_errors(controller)) continue;

            int n = Pm_Read(controller->stream, events, MIDI_BUFFER_SIZE);
            if(n < 0){
                WARN("MIDI Read error: %s\n", Pm_GetErrorText(n));
                continue;
            }
            for(int j = 0; j < n; j++)
            {
                PmMessage m = events[j].message;

                unsigned char event = Pm_MessageStatus(m);
                unsigned char data1 = Pm_MessageData1(m);
                unsigned char data2 = Pm_MessageData2(m);

                //printf("Device %d event %d %d %d %li - %d\n", i, event, data1, data2, (long int) events[j].timestamp, n_recent_events);

                for(int k = 0; k < controller->n_connections; k++)
                {
                    if(controller->connections[k].event == event && controller->connections[k].data1 == data1)
                    {
                        midi_command_event_data_t* event_data;
                        switch(controller->connections[k].type){
                            case MIDI_CONN_PARAM_STATE:
                                param_state_setq(controller->connections[k].param_state, data2 / 127.);
                                break;
                            case MIDI_CONN_SLOT_MUTE:
                                controller->connections[k].command_slot->mute = controller->connections[k].command_index;
                                break;
                            case MIDI_CONN_SLOT_SOLO:
                                controller->connections[k].command_slot->solo = controller->connections[k].command_index;
                                break;
                            case MIDI_CONN_PATTERN:
                                event_data = malloc(sizeof(midi_command_event_data_t));
                                if(!event_data) FAIL("Unable to malloc a MIDI command SDL event data\n");

                                event_data->slot = controller->connections[k].command_slot;
                                event_data->command.index = controller->connections[k].command_index;
                                event_data->command.status = controller->connections[k].command_status;
                                event_data->command.value = (float)data2 / 127;

                                SDL_Event sdl_event;
                                sdl_event.type = SDL_MIDI_COMMAND_EVENT;
                                sdl_event.user.code = 0;
                                sdl_event.user.data1 = event_data;
                                sdl_event.user.data2 = 0;
                                SDL_PushEvent(&sdl_event);
                                break;
                        }
                    }
                }
            }
        }
        SDL_Delay(1); // TODO SDL rate limiting
    }

    for(int i = 0; i < n_midi_controllers; i++)
    {
        if(!midi_controllers[i].stream) continue;
        err = Pm_Close(midi_controllers[i].stream);
        if(err != pmNoError) FAIL("Could not close MIDI device: %s\n", Pm_GetErrorText(err));
    }

    err = Pm_Terminate();
    if(err != pmNoError) FAIL("Could not terminate PortMIDI: %s\n", Pm_GetErrorText(err));

    return 0;
}

void midi_start()
{
    midi_running = 1;

    midi_thread = SDL_CreateThread(&midi_run,"Midi Thread", 0);
    if(!midi_thread) FAIL("Could not create MIDI thread: %s\n",SDL_GetError());
}

void midi_stop()
{
    midi_running = 0;

    SDL_WaitThread(midi_thread, 0);
}

