#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>

#include "core/err.h"
#include "core/slot.h"
#include "signals/signal.h"
#include "timebase/timebase.h"
#include "core/time.h"
#include "crc.h"
#include "lux.h"
#include "output/output.h"
#include "output/serial.h"
#include "output/slice.h"

#define LUX_WRITE_ONLY

static int output_running;

color_t** output_buffers = 0;

static SDL_Thread* output_thread;

static int output_run(void* args)
{
    PARAM_UNUSED args;
    struct lux_frame lf;
    char r;
    char need_delay;
    int c = 0;
    int last_tick;
    PARAM_UNUSED last_tick; 
    while(output_running){
        mbeat_t tb = timebase_get();

        update_patterns(tb);
        update_hits(tb);
        update_signals(tb);

        need_delay = 1;
        for(int i=0; i<n_output_strips; i++){
            if(output_strips[i].bus < 0) continue;

            need_delay = 0;

            output_to_buffer(&output_strips[i], output_buffers[i]);

            int j = 0;
            float energy = 0.;
            float scalar = 1.0;
            for(int k = 0; k < output_strips[i].length; k++){
                energy += output_buffers[i][k].r;
                energy += output_buffers[i][k].g;
                energy += output_buffers[i][k].b;
            }
            scalar = output_strips[i].length * 2.2 / energy * 255.;
            if(scalar > 255.)
                scalar = 255.;

            for(int k = 0; k < output_strips[i].length; k++){
                lf.data.carray.data[j++] = output_buffers[i][k].r * scalar;
                lf.data.carray.data[j++] = output_buffers[i][k].g * scalar;
                lf.data.carray.data[j++] = output_buffers[i][k].b * scalar;
            }
            lf.data.carray.cmd = CMD_FRAME; j++;
            lf.destination = output_strips[i].id;
            lf.length = j;

            if((r = lux_tx_packet(&lf)))
                printf("failed cmd: %d\n", r);

        }
        stat_ops = c++;
        SDL_Delay(1);
        if(need_delay){
            //printf("delaying output\n");
            SDL_Delay(10);
        }
        if(c % 100 == 0){
            int t = SDL_GetTicks();
            stat_ops = c;
            last_tick = t;
            printf("output %d - %d\n", c, SDL_GetTicks());
        }
    }
    return 0;
}

void output_start()
{
    output_buffers = malloc(sizeof(color_t*) * n_output_strips);

    if(!output_buffers) FAIL("Could not allocate output buffer array");

    for(int i=0; i<n_output_strips; i++)
    {
        output_buffers[i] = malloc(sizeof(color_t) * output_strips[i].length);
        if(!output_buffers[i]) FAIL("Could not allocate output buffer");
    }

    if(serial_init()){
        struct lux_frame cmd;
        struct lux_frame resp;
        char r;
        PARAM_UNUSED cmd;
        PARAM_UNUSED resp;
        PARAM_UNUSED r;
        printf("Serial initialized\n");
        output_running = 1;

        for(int i=0; i<n_output_strips; i++) {
#ifndef LUX_WRITE_ONLY
            cmd.data.carray.cmd = CMD_GET_ID;
            cmd.destination = output_strips[i].id;
            cmd.length = 1;

            if((r = lux_command_response(&cmd, &resp, 20))){
                printf("failed cmd get_id: %d\n", r);
                output_strips[i].bus = -1;
            }else{
                printf("Found light strip %d @0x%08x: '%s'\n", i, cmd.destination, resp.data.raw);
                output_strips[i].bus = 0;

                cmd.data.carray.cmd = CMD_GET_LENGTH;
                cmd.length = 1;
                if((r = lux_command_response(&cmd, &resp, 20))){
                    printf("failed cmd get_length: %d\n", r);
                }else{
                    if(output_strips[i].length == resp.data.ssingle_r.data)
                        printf("...length matches: %d\n", resp.data.ssingle_r.data);
                    else
                        printf("...length mis-match! %d in file, %d from strip\n", output_strips[i].length, resp.data.ssingle_r.data);
                }
            }
#else
            output_strips[i].bus = 0;
            printf("Attached light strip 0x%08x\n", output_strips[i].id);
#endif
        }
        output_thread = SDL_CreateThread(&output_run, 0);
        if(!output_thread) FAIL("Could not create output thread: %s\n",SDL_GetError());
    }else{
        printf("No serial initialized\n");
        for(int i = 0; i < n_output_strips; i++){
            output_strips[i].bus = -1;
        }
    }
}

void output_stop()
{
    output_running = 0;

    SDL_WaitThread(output_thread, 0);

    for(int i=0; i<n_output_strips; i++)
    {
        free(output_buffers[i]);
    }

    free(output_buffers);

    serial_close();
}

