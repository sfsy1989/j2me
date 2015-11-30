#ifndef __F_LOG_H__
#define __F_LOG_H__

#include <stdarg.h>
#define LOG_ERR		3
#define LOG_LOCAL0	0200
#define LOG_NOTICE	5
#define LOG_DEBUG	7
#define LOG_WARNING	4
#define FV_LOG_PRIMASK	7
#define FV_LOG_PRI(x)	((x) & FV_LOG_PRIMASK)

#ifdef __cplusplus
extern "C" {
#endif

#define F_LOG_FLIB      0x00000020
#define F_LOG_OLIB      0x00000040
#define F_LOG_NLIB      0x00000080
#define F_LOG_DRAW      0x00100000
#define F_LOG_IPC       0x00000010

enum{
	F_LOG_MODE_NULL = 0,
	F_LOG_MODE_PRINTF = 1,
	F_LOG_MODE_REDIRECT = 2,
	F_LOG_MODE_UDP = 3,
	F_LOG_MODE_SYSLOG = 4,
	F_LOG_MODE_CROSS_NAT = 5
};


int f_logger_init(char* name);
int f_logger_reinit();
void f_logger_uninit();
void f_logger(int priority, const char *fmt, ...);
void f_vlogger(int priority, const char *fmt, va_list ap);
void f_vlogger_line(int priority, const char *fmt, va_list ap);
void f_set_log_priority(unsigned int priority);
void f_set_log_facility(unsigned int facility);
void f_add_log_facility(unsigned int facility);
void f_remove_log_facility(unsigned int facility);
unsigned int f_get_log_facility();
unsigned int f_get_log_level();
void f_log_enable();
void f_log_disable();
fboolean f_get_log_facility_flag(unsigned int facility);
void f_set_log_mode(unsigned int mode, char *output, int port);

#ifdef FVT_BUILD_RELEASE
#define f_error(mod, fmt, ...) \
    f_logger(LOG_ERR | (mod), "%s - " fmt, __FUNCTION__, ##__VA_ARGS__)
#else
#define f_error(mod, fmt, ...) \
do {\
    f_logger(LOG_ERR | (mod), "%s - " fmt, __FUNCTION__, ##__VA_ARGS__);\
} while(0)
#endif

#define f_notice(mod, fmt, ...) \
    f_logger(LOG_NOTICE | (mod), "%s - " fmt, __FUNCTION__, ##__VA_ARGS__)
#define f_debug(mod, fmt, ...) \
    f_logger(LOG_DEBUG | (mod), "%s - " fmt, __FUNCTION__, ##__VA_ARGS__)
#define f_warning(mod, fmt, ...) \
    f_logger(LOG_WARNING | (mod), "%s - " fmt, __FUNCTION__, ##__VA_ARGS__)

#define flib_debug(...)    f_debug(F_LOG_FLIB,  __VA_ARGS__)
#define flib_error(...)    f_error(F_LOG_FLIB,  __VA_ARGS__)
#define flib_notice(...)   f_notice(F_LOG_FLIB, __VA_ARGS__)
#define flib_warning(...)   f_warning(F_LOG_FLIB, __VA_ARGS__)
#define olib_debug(...)    f_debug(F_LOG_OLIB,  __VA_ARGS__)
#define olib_error(...)    f_error(F_LOG_OLIB,  __VA_ARGS__)
#define olib_notice(...)   f_notice(F_LOG_OLIB, __VA_ARGS__)
#define nlib_debug(...)    f_debug(F_LOG_NLIB,  __VA_ARGS__)
#define nlib_error(...)    f_error(F_LOG_NLIB,  __VA_ARGS__)
#define nlib_notice(...)   f_notice(F_LOG_NLIB, __VA_ARGS__)
#define draw_debug(...)    f_debug(F_LOG_DRAW,  __VA_ARGS__)
#define draw_error(...)    f_error(F_LOG_DRAW,  __VA_ARGS__)
#define draw_notice(...)   f_notice(F_LOG_DRAW, __VA_ARGS__)
#define ipc_debug(...)    f_debug(F_LOG_IPC,  __VA_ARGS__)
#define ipc_error(...)    f_error(F_LOG_IPC,  __VA_ARGS__)
#define ipc_notice(...)   f_notice(F_LOG_IPC, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
