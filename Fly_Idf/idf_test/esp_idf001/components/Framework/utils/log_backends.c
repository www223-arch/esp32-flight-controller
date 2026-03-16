#include "log_backends.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// VOFA backend (uses vofa_send_rawdata)
extern void vofa_send_rawdata(uint8_t* data, size_t len);

static void vofa_backend(const char* level, const char* tag, const char* msg){
    char buf[240];
    int n = snprintf(buf, sizeof(buf), "%s/%s: %s", level, tag, msg);
    if(n>0) vofa_send_rawdata((uint8_t*)buf, (size_t)(n));
}

void framework_log_use_vofa_backend(void){
    framework_log_set_backend((void(*)(const char*,const char*,const char*))vofa_backend);
}

// File backend
static FILE* s_logfile = NULL;
static void file_backend(const char* level, const char* tag, const char* msg){
    if(!s_logfile) return;
    fprintf(s_logfile, "%s/%s: %s\n", level, tag, msg);
    fflush(s_logfile);
}
void framework_log_use_file_backend(const char* path){
    if(s_logfile) fclose(s_logfile);
    s_logfile = fopen(path, "a");
    if(s_logfile) framework_log_set_backend(file_backend);
}

// UDP backend (simple, best-effort)
#ifdef LWIP_SOCKETS
#include "lwip/sockets.h"
#include "lwip/inet.h"

static int s_udp_sock = -1;
static struct sockaddr_in s_udp_addr;

static void udp_backend(const char* level, const char* tag, const char* msg){
    if(s_udp_sock < 0) return;
    char buf[240]; int n = snprintf(buf,sizeof(buf),"%s/%s: %s",level,tag,msg);
    if(n>0) sendto(s_udp_sock, buf, n, 0, (struct sockaddr*)&s_udp_addr, sizeof(s_udp_addr));
}

void framework_log_use_udp_backend(const char* ip, uint16_t port){
    if(s_udp_sock >=0) { close(s_udp_sock); s_udp_sock=-1; }
    s_udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(s_udp_sock<0) return;
    memset(&s_udp_addr,0,sizeof(s_udp_addr));
    s_udp_addr.sin_family = AF_INET;
    s_udp_addr.sin_port = htons(port);
    s_udp_addr.sin_addr.s_addr = inet_addr(ip);
    framework_log_set_backend(udp_backend);
}
#else
void framework_log_use_udp_backend(const char* ip, uint16_t port){ (void)ip; (void)port; }
#endif

// TCP backend (connect on use)
#ifdef LWIP_SOCKETS
static int s_tcp_sock = -1;
static struct sockaddr_in s_tcp_addr;

static void tcp_backend(const char* level, const char* tag, const char* msg){
    if(s_tcp_sock < 0) return;
    char buf[240]; int n = snprintf(buf,sizeof(buf),"%s/%s: %s",level,tag,msg);
    if(n>0) send(s_tcp_sock, buf, n, 0);
}

void framework_log_use_tcp_backend(const char* ip, uint16_t port){
    if(s_tcp_sock>=0) { close(s_tcp_sock); s_tcp_sock=-1; }
    s_tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(s_tcp_sock<0) return;
    memset(&s_tcp_addr,0,sizeof(s_tcp_addr));
    s_tcp_addr.sin_family = AF_INET;
    s_tcp_addr.sin_port = htons(port);
    s_tcp_addr.sin_addr.s_addr = inet_addr(ip);
    if(connect(s_tcp_sock, (struct sockaddr*)&s_tcp_addr, sizeof(s_tcp_addr))<0){ close(s_tcp_sock); s_tcp_sock=-1; return; }
    framework_log_set_backend(tcp_backend);
}
#else
void framework_log_use_tcp_backend(const char* ip, uint16_t port){ (void)ip; (void)port; }
#endif
