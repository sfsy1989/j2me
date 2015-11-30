#ifndef __F_UTILS_H__
#define __F_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FV_IS_IP                   1
#define FV_PARSE_DOMAIN_SUCCESS    2
#define FV_PARSE_DOMAIN_FAILED     3

int f_encode_chars(char *in, char *out, int size);
int f_decode_chars(char *in, char *out, int size);
int f_json_get_int(const char *json, const char *key);
void f_json_get_string(const char *json, const char *key, char *getval);
void f_get_ustring_url(unsigned short *in_string, char *out_string, int size);
int f_get_env_int(const char *param);
int f_get_env_string(const char *param, char *val, int len);
int f_check_domain(char *domain);
int f_get_ip_from_url(char *url, char *ip, int size);
fboolean f_string_empty(const char *str);
int f_get_url_head(char *url, char *head, int size);
fboolean f_check_url(const char *url);

#ifdef __cplusplus
}
#endif

#endif

