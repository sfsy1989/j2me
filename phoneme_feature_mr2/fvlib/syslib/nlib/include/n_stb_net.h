#ifndef __F_STB_NET_H__
#define __F_STB_NET_H__

#define cmd_net_get_netwire_state "net.GetNetwireState"

#define WIRELESS_PARAMS_SSID_SIZE 256
#define WIRELESS_PARAMS_BSSID_SIZE 20
#define WIRELESS_PARAMS_Security_SIZE 32
#define WIRELESS_PARAMS_WMODE_SIZE 16
#define WIRELESS_PARAMS_EXTCH_SIZE 8
#define WIRELESS_PARAMS_NT_SIZE   8
#define WIRELESS_PARAMS_NUME_SIZE   8


#define WIRELESS_SEARCH_COUNT_MAX 12


enum{
	WIRELESS_AUTH_TYPE_OPEN = 1,
	WIRELESS_AUTH_TYPE_SHARE,
	WIRELESS_AUTH_TYPE_WPAPSK,
	WIRELESS_AUTH_TYPE_WPA2PSK,
	WIRELESS_AUTH_TYPE_WPANONE,
	WIRELESS_AUTH_TYPE_WEPAUTO
};

enum{
	WIRELESS_ENC_TYPE_NONE = 1,
	WIRELESS_ENC_TYPE_WEP,
	WIRELESS_ENC_TYPE_TKIP,
	WIRELESS_ENC_TYPE_AES
};


typedef enum {
	F_NET_CABLE_OFF = 0,
	F_NET_CABLE_ON,
	F_NET_IP_ASSIGNED,
	F_NET_IP_RELEASED,	///< ip address is released
	F_NET_START_CONNECT
} f_net_stat_e;

typedef struct wireless_params{
	char ch[WIRELESS_PARAMS_NUME_SIZE];
	char ssid[WIRELESS_PARAMS_SSID_SIZE];
	char BSSID[WIRELESS_PARAMS_BSSID_SIZE];
	char security[WIRELESS_PARAMS_Security_SIZE];
    char signal[WIRELESS_PARAMS_NUME_SIZE];
	char w_mode[WIRELESS_PARAMS_WMODE_SIZE];
	char ExtCH[WIRELESS_PARAMS_EXTCH_SIZE];
	char NT[WIRELESS_PARAMS_NT_SIZE];
}wireless_params_s;


typedef struct wireless_search{
	wireless_params_s search_list[WIRELESS_SEARCH_COUNT_MAX];
	int search_count;
}wireless_search_s;

int n_net_get_netwire_state(int *state);

#endif
