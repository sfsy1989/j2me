#ifndef __N_IPTV_CHANLIST_H__
#define __N_IPTV_CHANLIST_H__

#include "n_limit.h"

struct ChannelLogoStruct {
	char ChannelLogoURL[N_IPTV_CHANNEL_URL_LEN+1];
	int PositionX;
	int PositionY;
	int BeginTime;
	int Interval;
	int Lasting;
};

#define CHANNEL_TYPE_WEB "type3"
typedef struct ChannelInfoStruct {
	char ChannelID[N_IPTV_CHANNEL_ID_LEN+1];
	char ChannelName[N_IPTV_CHANNEL_NAME_LEN+1];
	int UserChannelID;
	char ChannelURL[N_IPTV_CHANNEL_URL_LEN+1];
	int TimeShift;
	char ChannelSDP[N_IPTV_CHANNEL_URL_LEN+1];
	char TimeShiftURL[N_IPTV_CHANNEL_URL_LEN+1];
	struct ChannelLogoStruct ChannelLogo;
	char Channeltype[N_IPTV_CHANNEL_TYPE_LEN+1];
	int ChannelPurchased;
	int fec_port;
	char ChannelFCCIP[N_IPTV_CHANNEL_IP_LEN+1];
	int ChannelFCCPort;
} ChannelInfoStruct;

#define CHANLIST_DAT_FILE  "/tmp/channellist"

int n_iptv_chanlist_init(int count);
void n_iptv_chanlist_uninit();
int n_iptv_chanlist_attach();
int n_iptv_chanlist_detach();
int n_iptv_chanlist_get_count();
int n_iptv_chanlist_add_chan(ChannelInfoStruct *chan_info);
ChannelInfoStruct *n_iptv_chanlist_get_chan(int chan_id);

#endif

