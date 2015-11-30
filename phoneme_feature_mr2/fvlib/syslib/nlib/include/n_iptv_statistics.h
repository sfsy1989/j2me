#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <sys/types.h>
#include "n_limit.h"

typedef struct {
	int16_t AuthNumbers;
	int16_t AuthFailNumbers;    /// \bug can not get from ipanel or epg
	char    AuthFailInfo[10][F_IPTV_INFO_STR_SIZE];  /// \bug can not get from ipanel or epg

	int16_t HTTPReqNumbers;     /// \bug can not get from ipanel
	int16_t HTTPFailNumbers;
	char    HTTPFailInfo[10][F_IPTV_INFO_STR_SIZE];
} brows_iptv_stat_s;

typedef struct {
	int16_t MultiReqNumbers;
	int16_t MultiFailNumbers;
	char MultiFailInfo[10][F_IPTV_INFO_STR_SIZE];
	int16_t VodReqNumbers;
	int16_t VodFailNumbers;
	char VodFailInfo[10][F_IPTV_INFO_STR_SIZE];
	int16_t MutiAbendNumbers;
	int16_t VODAbendNumbers;
	int16_t PlayErrorNumbers;
	char PlayErrorInfo[10][F_IPTV_INFO_STR_SIZE];

	int16_t MultiPacketsLostR[F_TR069_IPTV_PACK_LOST_CNT_SIZE];
	int16_t VODPacketsLostR[F_TR069_IPTV_PACK_LOST_CNT_SIZE];
	int16_t MultiBitRateR[F_TR069_IPTV_RATE_STR_SIZE];
	int16_t VODBitRateR[F_TR069_IPTV_RATE_STR_SIZE];
	int16_t FramesLostR[F_TR069_IPTV_FRAME_LOST_CNT_SIZE];
} mplayer_iptv_statistics_s;

typedef struct {
	unsigned int ConnectionUpTime;
	unsigned int TotalBytesSent;
	unsigned int TotalBytesReceived;
	unsigned int TotalPacketsSent;
	unsigned int TotalPacketsReceived;
	unsigned int  CurrentDayInterval;
	unsigned int CurrentDayBytesSent;
        unsigned int CurrentDayBytesReceived;
	unsigned int CurrentDayPacketsSent;
	unsigned int CurrentDayPacketsReceived;
	unsigned int  QuarterHourInterval;
	unsigned int  QuarterHourBytesSent;
	unsigned int  QuarterHourBytesReceived;
	unsigned int  QuarterHourPacketsSent;
	unsigned int QuarterHourPacketsReceived;
}  lan_statistics_s;

/*
 * IPTV Log Facility
 */
typedef enum {
    /* status change */
	IET_POWER_ON = 0,
	IET_POWER_OFF,
    IET_DEV_CHG,
    IET_STANDBY,
    IET_RESUME,
    /* failure */
    IET_SOFTWARE_FAIL,
    IET_HARDWARE_FAIL,
    IET_NET_FAIL,
    /* service */
    IET_SERVICE,
    /* configuration */
    IET_LOCAL_CFG,
    IET_REMOTE_CFG,
    /* others */
    IET_OTHERS
} f_iptv_event_type;

typedef enum {
	IFL_DEBUG = 0,
	IFL_INFO,
	IFL_WARN,
	IFL_ERROR,
	IFL_FATAL
} f_iptv_fault_level;

typedef enum {
	STB_ERR_NONE                        = -1,
	STB_ERR_LOAD_SYSTEM_CHECK           = 0,	/* ���������Լ� */
	STB_ERR_LOAD_SYSTEM_LOAD            = 10,	/* ϵͳ���� */
	STB_ERR_LOAD_NET_UNLINK             = 20,	/* ���߼�� */
	STB_ERR_LOAD_NETWORK_TIMEOUT        = 30,	/* �������40S��ʱ */
	STB_ERR_LOAD_NETWORK_LOGIN          = 31,	/* ��������û������������ */
	STB_ERR_LOAD_NETWORK_RESPOND        = 32,	/* ������������û��Ӧ */
	STB_ERR_LOAD_SOAP_TIMEOUT           = 40,	/* soap����7S��ʱ����Ӧ */
	STB_ERR_LOAD_HTTP_TIMEOUT           = 50,	/* http����7S��ʱ����Ӧ */
	STB_ERR_LOAD_HTTP_NOTFOUND          = 51,       /* http 404 not found */
	STB_ERR_LOAD_HTTP_SERVER            = 52,       /* http server error */
	STB_ERR_LOAD_HTTP_ABNORMAL          = 53,       /* http abnormal */
	STB_ERR_RTSP_CON_TIMEOUT            = 100,	/* �㲥����7S��ʱ����Ӧ */
	STB_ERR_RTSP_CON_RESPOND            = 101,	/* �㲥���ӷ���������Ӧ */
	STB_ERR_RTSP_CON_SERVER_BANDWIDTH   = 102,	/* �㲥���ӷ����������� */
	STB_ERR_RTSP_CON_SERVER_BUSY        = 103,	/* �㲥���ӷ�����æ */
	STB_ERR_RTSP_CON_LOCAL_BANDWIDTH    = 104,	/* �㲥���ӽ�������� */
	STB_ERR_RTSP_CON_CONTENT_NOEXIST    = 105,	/* �㲥���ݲ����� */
	STB_ERR_RTSP_CON_PROGRAM_LINK       = 106,	/* �㲥��Ŀ���Ӵ��� */
	STB_ERR_RTSP_CON_FORMAT_NOT_SUPPORT = 107,	/* �㲥��֧�����ݸ�ʽ */
	STB_ERR_RTSP_CON_PERMISSION         = 108,	/* �㲥�û���Ȩ�� */
	STB_ERR_RTSP_CON_OUTLAY             = 109,	/* �㲥�û����ò��� */
	STB_ERR_RTSP_PLAY_DECODE            = 120,	/* �㲥����ʧ�� */
	STB_ERR_RTSP_PLAY_DATA              = 121,	/* �㲥���ղ������� */
	STB_ERR_RTSP_OP_DEFAULT_TIMEOUT     = 130,	/* �㲥��������3S��ʱ */
	STB_ERR_RTSP_OP_FORWARD_TIMEOUT     = 131,	/* �㲥���3S��ʱ����Ӧ */
	STB_ERR_RTSP_OP_REWIND_TIMEOUT      = 132,	/* �㲥����3S��ʱ����Ӧ */
	STB_ERR_RTSP_OP_SEEK_TIMEOUT        = 133,	/* �㲥��λ3S��ʱ����Ӧ */
	STB_ERR_RTSP_OP_PAUSE_TIMEOUT       = 134,	/* �㲥��ʱ3S��ʱ����Ӧ */
	STB_ERR_IGMP_CON_TIMEOUT            = 200,	/* �鲥����7S��ʱ����Ӧ */
	STB_ERR_IGMP_CON_RESPOND            = 201,	/* �鲥����������Ӧ */
	STB_ERR_IGMP_CON_DATASOURCE         = 204,	/* �鲥Դ������ */
	STB_ERR_IGMP_CON_LOCAL_BANDWIDTH    = 205,	/* �鲥��������� */
	STB_ERR_IGMP_CON_PROGRAM_LINK       = 206,	/* �鲥��Ŀ���Ӵ��� */
	STB_ERR_IGMP_CON_FORMAT_NOT_SUPPORT = 207,	/* �鲥��֧�����ݸ�ʽ */
	STB_ERR_IGMP_CON_PERMISSION         = 208,	/* �鲥�û���Ȩ�� */
	STB_ERR_IGMP_CON_OUTLAY             = 209,	/* �鲥�û����ò��� */
	STB_ERR_IGMP_PLAY_DECODE            = 220,	/* �鲥����ʧ�� */
	STB_ERR_IGMP_PLAY_DATA              = 221,	/* ���鲥���ݣ�3S��ʱ */
	STB_ERR_IGMP_OP_DEFAULT_TIMEOUT     = 230,	/* �鲥��������3S��ʱ����Ӧ */
	STB_ERR_IGMP_OP_FORWARD_TIMEOUT     = 231,	///< �鲥���3S��ʱ
	STB_ERR_IGMP_OP_REWIND_TIMEOUT      = 232,	///< �鲥����3S��ʱ
	STB_ERR_IGMP_OP_SEEK_TIMEOUT        = 233,	///< �鲥��λ3S��ʱ
	STB_ERR_IGMP_OP_PAUSE_TIMEOUT       = 234,	///< �鲥��ͣ3S��ʱ
	MAX_ERROR
} f_stb_error_code;

#endif
