#ifndef _f_traceroute_
#define _f_traceroute_
#include<netinet/in.h>

//enum{
//	F_PING_MSG = 0,
//	F_TRCERT_MSG = 1,
//};
#define F_TRCERT_MSG 1
void f_tracermsg(int pingortrcert, const char *fmt, ...);

#define f_trcert_msg(pingortrcert,fmt,...)    f_tracermsg(pingortrcert,"%s --" fmt, __FUNCTION__, ##__VA_ARGS__)
#define ftracer_msg(...)	f_trcert_msg(F_TRCERT_MSG,__VA_ARGS__)

/* argument pass to tracert */
typedef struct _f_tracert_argument
{
	/* max hop count */
	int max_hop_count;
	/* time out */
	int time_out;
	/* data block size */
	int data_size;
}f_tracert_argument;

/* result return from tracert */
typedef struct _f_tracert_result
{
	int response_time;
	int hop_number;
	char hophost[32][256];
	int getdstip;
} f_tracert_result;

/* Host name and address list */
struct hostinfo {
	char *name;
	int n;
	u_int32_t *addrs;
};

int f_traceroute(const char *host, f_tracert_argument *argument, f_tracert_result *result);
void tvsub(struct timeval *out, struct timeval *in);
void f_print(unsigned char *buf, int cc,struct sockaddr_in *from,int i, f_tracert_result *result);
u_short in_cksum(u_short *addr, int len);
void *xcalloc(size_t nmemb, size_t size);
struct hostinfo * gethostinfo(const char *host);
void freehostinfo(struct hostinfo *hi);
void f_trace_transsocket( int sock ,char *pcip);

#endif
