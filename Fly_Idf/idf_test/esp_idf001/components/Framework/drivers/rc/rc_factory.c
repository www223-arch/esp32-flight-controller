#include "rc.h"
#include "../../Middlewares/PS2/ps2.h"
#include <stdlib.h>
#include <string.h>

static bool sbus_init(rc_dev_t* r){ (void)r; return true; }
static int sbus_read(rc_dev_t* r, float* chans, int max_chans){ (void)r; if(max_chans>8) max_chans=8; for(int i=0;i<max_chans;i++) chans[i]=0.5f; return max_chans; }
static void sbus_deinit(rc_dev_t* r){ (void)r; }

rc_dev_t* rc_factory_create(const char* name){
    rc_dev_t* r = malloc(sizeof(*r));
    if(!r) return NULL;
    memset(r,0,sizeof(*r));
    if(strcmp(name,"sbus")==0){
        r->init = sbus_init; r->read_channels = sbus_read; r->deinit = sbus_deinit; r->name = "sbus";
    }else if(strcmp(name,"ps2")==0){
        // Adapter for existing PS2 C driver
        static bool ps2_init_wrapper(rc_dev_t* rr){ (void)rr; PS2_Init(); return true; }
        static int ps2_read_wrapper(rc_dev_t* rr, float* chans, int max_chans){
            (void)rr;
            if(max_chans > 4) max_chans = 4;
            chans[0] = PS2_AnologData(PSS_LX) / 255.0f;
            chans[1] = PS2_AnologData(PSS_LY) / 255.0f;
            chans[2] = PS2_AnologData(PSS_RX) / 255.0f;
            chans[3] = PS2_AnologData(PSS_RY) / 255.0f;
            return 4;
        }
        static void ps2_deinit_wrapper(rc_dev_t* rr){ (void)rr; }
        r->init = ps2_init_wrapper; r->read_channels = ps2_read_wrapper; r->deinit = ps2_deinit_wrapper; r->name = "ps2";
    }else{
        free(r); return NULL;
    }
    return r;
}

void rc_factory_destroy(rc_dev_t* r){ if(!r) return; if(r->deinit) r->deinit(r); free(r); }
