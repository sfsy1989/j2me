#ifndef __N_OPERATION_LOG_H__
#define __N_OPERATION_LOG_H__

#include <stdarg.h>
#include <syslog.h>

#ifdef __cplusplus
extern "C" {
#endif

enum{
	N_LOG_MODE_NULL = 0,
	N_LOG_MODE_PRINTF = 1,
	N_LOG_MODE_REDIRECT = 2,
	N_LOG_MODE_UDP = 3,
	N_LOG_MODE_SYSLOG = 4,
	N_LOG_MODE_CROSS_NAT = 5
};

void n_operation_log(int priority, const char *fmt, ...);
void n_set_log_priority(unsigned int priority);
void n_set_log_facility(unsigned int facility);
unsigned int n_get_log_facility();
unsigned int n_get_log_level();
void n_log_enable();
void n_log_disable();
fboolean n_get_log_facility_flag(unsigned int facility);
void n_add_log_facility(unsigned int facility);
void n_remove_log_facility(unsigned int facility);
void n_set_log_mode(unsigned int mode, char *output, int port);

#define n_operation_error(mod, fmt, ...) \
    n_operation_log(LOG_ERR | (mod), fmt, ##__VA_ARGS__)
#define n_operation_notice(mod, fmt, ...) \
    n_operation_log(LOG_NOTICE | (mod), fmt, ##__VA_ARGS__)
#define n_operation_debug(mod, fmt, ...) \
    n_operation_log(LOG_DEBUG | (mod), fmt, ##__VA_ARGS__)
#define n_operation_warning(mod, fmt, ...) \
    n_operation_log(LOG_WARNING | (mod), fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

