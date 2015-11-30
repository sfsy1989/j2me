#ifndef __FLIB_PING_H__
#define __FLIB_PING_H__


//enum{
//	F_PING_MSG = 0,
//	F_TRCERT_MSG = 1,
//};
#define F_PING_MSG 0
void f_pingmsg(int pingortrcert, const char *fmt, ...);

#define f_ping_msg(pingortrcert,fmt,...)    f_pingmsg(pingortrcert,"%s -- " fmt, __FUNCTION__, ##__VA_ARGS__)
#define fping_msg(...)	f_ping_msg(F_PING_MSG,__VA_ARGS__)

/* result return from ping */
typedef struct _tr069_ping_result
{
	/* success count */
	int success_count;
	/* failure count */
	int failure_count;
	/* average response time */
	float avg_time;
	/* minimum response time */
	float min_time;
	/* maximum respones time */
	float max_time;
}tr069_ping_result;

typedef struct _ping_msg
{
	/* success count */
	int success_count;
	/* failure count */
	int failure_count;
	/* average response time */
	float avg_time;
	/* minimum response time */
	float min_time;
	/* maximum respones time */
	float max_time;
	unsigned long ipaddr;
	int sz;
	unsigned short recv_seq;
	int ttl;
	unsigned long triptime;
	int udp;
	int pingflag;
	unsigned long icmp_type;
	char icmp_type_name[30];
	int transmitted;
}ping_msg;
int f_ping(const char *host,int f_ping_count,int f_max_wait,int f_data_len,tr069_ping_result *f_result);
int f_ping1(const char *host,int f_ping_count,int f_max_wait,int f_data_len,ping_msg *f_ping_result);
void f_reset_ping_msg();
void f_ping_transsocket( int sock ,char *pcip);
#endif
