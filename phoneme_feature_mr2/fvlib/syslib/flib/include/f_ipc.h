#ifndef __F_IPC_H__
#define __F_IPC_H__

#define F_IPC_TIMER_REALTIME    5
#define F_IPC_TIMER_QUICK       200
#define F_IPC_TIMER_NORMAL      500
#define F_IPC_TIMER_IDLE        1000

#define MSG_TYPE_SYNC	0
#define MSG_TYPE_ASYNC	1

#define MAX_IF_NAME_SIZE 64
#define MSG_QUEUE_SIZE 64
#define IF_LIST_SIZE 64
#define F_IPC_MAX_VALUESET_SIZE 4096
#define F_IPC_MAX_VALUESET_NUM 16
#define MAX_PATH_LEN 32
#define MAX_WAIT_PROCCESS_SEC 10
#define MAX_WAIT_RETURN_MSEC 100
#define WAIT_TIME_INFINITE -1
#define MAX_MSG_RUNONCE 2

typedef struct _ipc_value_info {
	int type;
	int len;
	int offset;
} ipc_value_info;

typedef struct _ipc_valueset {
	int value_num;
	char value_content[F_IPC_MAX_VALUESET_SIZE];
	ipc_value_info value_list[F_IPC_MAX_VALUESET_NUM];
} ipc_valueset;

typedef int (*ipc_func) (ipc_valueset * arg, ipc_valueset * ret);

typedef struct {
	char if_name[MAX_IF_NAME_SIZE];
	int type;
	int seq;
} f_fvipc_header_s;

typedef union {
	ipc_valueset arg;
	ipc_valueset ret;
} f_fvipc_param_u;

typedef struct _ipc_msg {
	unsigned int id_code;
	f_fvipc_header_s header;
	f_fvipc_param_u param;
} ipc_msg;

typedef struct _ipc_interface {
	char if_name[MAX_IF_NAME_SIZE];
	ipc_func if_callback;
} ipc_interface;

typedef struct _ipc_service {
	int if_num;
	ipc_interface if_list[IF_LIST_SIZE];
	int listen_sock;
	unsigned short port;
	unsigned char local_only;
	unsigned char ignore_msg;
} ipc_service;

int ipc_form_valueset(ipc_valueset *value_set, char *list_string, ...);
int ipc_deform_valueset(ipc_valueset *value_set, char *list_string, ...);
ipc_service *ipc_init_service(unsigned short port, int local_only);
void ipc_uninit_service(ipc_service *service);
int ipc_add_interface(ipc_service *service, char *if_name, ipc_func if_callback);
int ipc_remove_interface(ipc_service *service, char *if_name);
int ipc_runonce_service(ipc_service *service);
int ipc_callsync(int svr_port, char *if_name, ipc_valueset *arg, ipc_valueset *ret);
int ipc_callasync(int svr_port, char *if_name, ipc_valueset *arg);
int ipc_wait_socket(int sockfd, char rd, char wr, int time_ms);

#endif
