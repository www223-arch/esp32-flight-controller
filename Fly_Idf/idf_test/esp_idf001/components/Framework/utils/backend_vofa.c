#include "log.h"
#include "../../Middlewares/MyVofa/vofa.h"
#include <stdio.h>

// VOFA backend: send log line via vofa_send_fmt
static void vofa_backend(const char* level, const char* tag, const char* msg){
    // format: LEVEL|TAG|MSG
    // build safe string
    char buf[200];
    snprintf(buf, sizeof(buf), "%s|%s|%s", level, tag, msg);
    vofa_send_rawdata((uint8_t*)buf, strlen(buf));
}

void register_vofa_backend(){
    framework_log_set_backend(vofa_backend);
}
