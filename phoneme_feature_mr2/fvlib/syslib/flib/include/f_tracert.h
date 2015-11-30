#ifndef _f_tracert_
#define _f_tracert_

#include "f_traceroute.h"

#define F_TRCERT_MSG1 1
void f_tracermsg1(int pingortrcert, const char *fmt, ...);

#define f_trcert_msg1(pingortrcert,fmt,...)    f_tracermsg1(pingortrcert,"%s --" fmt, __FUNCTION__, ##__VA_ARGS__)
#define ftracer_msg1(...)	f_trcert_msg1(F_TRCERT_MSG1,__VA_ARGS__)

struct f_proto {
    const char*(*icmpcode)(int);
    int (*recv)(int,struct timeval*);
    int (*send)(int sockfd, int datalen);
    struct sockaddr *sasend;    //dest addr, the destination
    struct sockaddr *sarecv;    //recv addr, store who send the message
    struct sockaddr *salast;
    socklen_t salen;
    int icmpproto;
    int ttllevel;
    int ttloptname;
} *pr;


int f_tracert(const char *host, f_tracert_argument *argument, f_tracert_result *result);
void f_trace_transsocket1( int sock ,char *pcip);

#endif

