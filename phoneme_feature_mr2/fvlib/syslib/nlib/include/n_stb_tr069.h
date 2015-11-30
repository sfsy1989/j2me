/********************************************************************
	Created By Ash, 2009/11/25
	Model: Tr069
	Sub Model: Tr069
	Function: Public definition and model entry
********************************************************************/
#ifndef __N_STB_TR069__
#define __N_STB_TR069__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <iniparser.h>
#include <semaphore.h>
#include <string.h>

#include "n_stb_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F_TR069_SESSION_ID_SIZE	128

/* max parameter number */
#define TR069_MAX_PARAM_NUM 300
/* max parameter name length */
#define TR069_MAX_PARAM_NAME_LEN 256
/* max parameter value length */
#define TR069_MAX_PARAM_VALUE_LEN 1024
/* max parameter code length */
#define TR069_MAX_PARAM_CODE_LEN 8
/* max fault code length */
#define TR069_MAX_FAULT_CODE_LEN 16
/* max inform parameter number */
#define TR069_MAX_INFORM_PARAM_NUM 32
/* time(usec) wait for service interface */
#define TR069_MAX_WAIT_SERVICE_TIME_US -1
/* time(usec) wait for tr069 initialization */
#define TR069_MAX_WAIT_INIT_TIME_US 5000000
/* time(msec) session semaphore can hold */
#define TR069_MAX_RESET_TIME_MS 15000

/* urn used by forming soap header */
#define TR069_URN "urn:dslforum-org:cwmp-1-0"

/* authentication flag */
#define TR069_AUTH

/* tr069 ini file name */
#define TR069_STATUS_FILENAME N_STB_PARAM_PATH"/tr069_status.ini"

/* parameter code, only inform parameters have */
#define PCODE_Manufacturer "\x21"
#define PCODE_ManufacturerOUI "\x22"
#define PCODE_ProductClass "\x23"
#define PCODE_SerialNumber "\x24"
#define PCODE_HardwareVersion "\x25"
#define PCODE_SoftwareVersion "\x26"
#define PCODE_ProvisioningCode "\x27"
#define PCODE_DeviceStatus "\x28"
#define PCODE_KernelVersion "\x29"
#define PCODE_FsVersion "\x2a"
#define PCODE_AppVersion "\x2b"
#define PCODE_StrmCtlProtocols "\x2c"
#define PCODE_StrmTrsptProtocols "\x2d"
#define PCODE_StrmTrsptCtlProtocols "\x2e"
#define PCODE_DldTrsptProtocols "\x2f"
#define PCODE_MultiplexTypes "\x30"
#define PCODE_MaxDjtBufferSize "\x31"
#define PCODE_AudioStandards "\x32"
#define PCODE_VideoStandards "\x33"
#define PCODE_ManageSerURL "\x34"
#define PCODE_ManageSerUsername "\x35"
#define PCODE_ManageSerPassword "\x36"
#define PCODE_ConnectionRequestURL "\x37"
#define PCODE_ConnectionRequestUsername "\x38"
#define PCODE_ConnectionRequestPassword "\x39"
#define PCODE_PeriodicInformEnable "\x3a"
#define PCODE_PeriodicInformInterval "\x3b"
#define PCODE_PeriodicInformTime "\x3c"
#define PCODE_UpgradesManaged "\x3d"
#define PCODE_ParameterKey "\x3e"
#define PCODE_ManageSerURLBackup "\x3f"
#define PCODE_NTPServer1 "\x40"
#define PCODE_LocalTimeZone "\x41"
#define PCODE_CurrentLocalTime "\x42"
#define PCODE_Device "\x43"
#define PCODE_DhcpUser "\x44"
#define PCODE_DhcpPassword "\x45"
#define PCODE_AddressingType "\x46"
#define PCODE_IPAddressReal "\x47"
#define PCODE_SubnetMaskReal "\x48"
#define PCODE_DefaultGatewayReal "\x49"
#define PCODE_DNSServer0Real "\x4a"
#define PCODE_DNSServer1Real "\x4b"
#define PCODE_MACAddress "\x4c"
#define PCODE_ConnectionUpTime "\x4d"
#define PCODE_TotalBytesSent "\x4e"
#define PCODE_TotalBytesReceived "\x4f"
#define PCODE_TotalPacketsSent "\x50"
#define PCODE_TotalPacketsReceived "\x51"
#define PCODE_CurrentDayInterval "\x52"
#define PCODE_CurrentDayBytesSent "\x53"
#define PCODE_CurrentDayBytesReceived "\x54"
#define PCODE_CurrentDayPacketsSent "\x55"
#define PCODE_CurrentDayPacketsReceived "\x56"
#define PCODE_QuarterHourInterval "\x57"
#define PCODE_QuarterHourBytesSent "\x58"
#define PCODE_QuarterHourBytesReceived "\x59"
#define PCODE_QuarterHourPacketsSent "\x5a"
#define PCODE_QuarterHourPacketsReceived "\x5b"
#define PCODE_PingDiagState "\x5c"
#define PCODE_PingDiagHost "\x5d"
#define PCODE_PingDiagReptNumber "\x5e"
#define PCODE_PingDiagTimeout "\x5f"
#define PCODE_PingDiagDataBlockSize "\x60"
#define PCODE_PingDiagDSCP "\x61"
#define PCODE_PingDiagSuccessCount "\x62"
#define PCODE_PingDiagFailureCount "\x63"
#define PCODE_PingDiagAvgRespTime "\x64"
#define PCODE_PingDiagMinRespTime "\x65"
#define PCODE_PingDiagMaxRespTime "\x66"
#define PCODE_TracertDiagState "\x67"
#define PCODE_TracertDiagHost "\x68"
#define PCODE_TracertDiagTimeout "\x69"
#define PCODE_TracertDiagDataBlockSize "\x6a"
#define PCODE_TracertDiagMaxHopCount "\x6b"
#define PCODE_TracertDiagDSCP "\x6c"
#define PCODE_TracertDiagRespTime "\x6d"
#define PCODE_TracertDiagHopsNumber "\x6e"
#define PCODE_HOPHOST1 "\x6f"
#define PCODE_HOPHOST2 "\x70"
#define PCODE_HOPHOST3 "\x71"
#define PCODE_HOPHOST4 "\x72"
#define PCODE_HOPHOST5 "\x73"
#define PCODE_HOPHOST6 "\x74"
#define PCODE_HOPHOST7 "\x75"
#define PCODE_HOPHOST8 "\x76"
#define PCODE_HOPHOST9 "\x77"
#define PCODE_HOPHOST10 "\x78"
#define PCODE_HOPHOST11 "\x79"
#define PCODE_HOPHOST12 "\x7a"
#define PCODE_HOPHOST13 "\x7b"
#define PCODE_HOPHOST14 "\x7c"
#define PCODE_HOPHOST15 "\x7d"
#define PCODE_HOPHOST16 "\x7e"
#define PCODE_HOPHOST17 "\x7f"
#define PCODE_HOPHOST18 "\x80"
#define PCODE_HOPHOST19 "\x81"
#define PCODE_HOPHOST20 "\x82"
#define PCODE_HOPHOST21 "\x83"
#define PCODE_HOPHOST22 "\x84"
#define PCODE_HOPHOST23 "\x85"
#define PCODE_HOPHOST24 "\x86"
#define PCODE_HOPHOST25 "\x87"
#define PCODE_HOPHOST26 "\x88"
#define PCODE_HOPHOST27 "\x89"
#define PCODE_HOPHOST28 "\x8a"
#define PCODE_HOPHOST29 "\x8b"
#define PCODE_HOPHOST30 "\x8c"
#define PCODE_HOPHOST31 "\x8d"
#define PCODE_HOPHOST32 "\x8e"
#define PCODE_STBID "\x8f"
#define PCODE_PPPoEID "\x90"
#define PCODE_PPPoEPassword "\x91"
#define PCODE_UserID "\x92"
#define PCODE_UserPassword "\x93"
#define PCODE_AuthURL "\x94"
#define PCODE_LogServerUrl "\x95"
#define PCODE_LogUploadInterval "\x96"
#define PCODE_LogRecordInterval "\x97"
#define PCODE_StatInterval "\x98"
#define PCODE_PacketsLostR1 "\x99"
#define PCODE_PacketsLostR2 "\x9a"
#define PCODE_PacketsLostR3 "\x9b"
#define PCODE_PacketsLostR4 "\x9c"
#define PCODE_PacketsLostR5 "\x9d"
#define PCODE_BitRateR1 "\x9e"
#define PCODE_BitRateR2 "\x9f"
#define PCODE_BitRateR3 "\xa0"
#define PCODE_BitRateR4 "\xa1"
#define PCODE_BitRateR5 "\xa2"
#define PCODE_FramesLostR1 "\xa3"
#define PCODE_FramesLostR2 "\xa4"
#define PCODE_FramesLostR3 "\xa5"
#define PCODE_FramesLostR4 "\xa6"
#define PCODE_FramesLostR5 "\xa7"
#define PCODE_Startpoint "\xa8"
#define PCODE_Endpoint "\xa9"
#define PCODE_AuthNumbers "\xaa"
#define PCODE_AuthFailNumbers "\xab"
#define PCODE_AuthFailInfo "\xac"
#define PCODE_MultiReqNumbers "\xad"
#define PCODE_MultiFailNumbers "\xae"
#define PCODE_MultiFailInfo "\xaf"
#define PCODE_VodReqNumbers "\xb0"
#define PCODE_VodFailNumbers "\xb1"
#define PCODE_VodFailInfo "\xb2"
#define PCODE_HTTPReqNumbers "\xb3"
#define PCODE_HTTPFailNumbers "\xb4"
#define PCODE_HTTPFailInfo "\xb5"
#define PCODE_MutiAbendNumbers "\xb6"
#define PCODE_VODAbendNumbers "\xb7"
#define PCODE_PlayErrorNumbers "\xb8"
#define PCODE_MultiPacketsLostR1Nmb "\xb9"
#define PCODE_MultiPacketsLostR2Nmb "\xba"
#define PCODE_MultiPacketsLostR3Nmb "\xbb"
#define PCODE_MultiPacketsLostR4Nmb "\xbc"
#define PCODE_MultiPacketsLostR5Nmb "\xbd"
#define PCODE_VODPacketsLostR1Nmb "\xbe"
#define PCODE_VODPacketsLostR2Nmb "\xbf"
#define PCODE_VODPacketsLostR3Nmb "\xc0"
#define PCODE_VODPacketsLostR4Nmb "\xc1"
#define PCODE_VODPacketsLostR5Nmb "\xc2"
#define PCODE_MultiBitRateR1Nmb "\xc3"
#define PCODE_MultiBitRateR2Nmb "\xc4"
#define PCODE_MultiBitRateR3Nmb "\xc5"
#define PCODE_MultiBitRateR4Nmb "\xc6"
#define PCODE_MultiBitRateR5Nmb "\xc7"
#define PCODE_VODBitRateR1Nmb "\xc8"
#define PCODE_VODBitRateR2Nmb "\xc9"
#define PCODE_VODBitRateR3Nmb "\xca"
#define PCODE_VODBitRateR4Nmb "\xcb"
#define PCODE_VODBitRateR5Nmb "\xcc"
#define PCODE_FramesLostR1Nmb "\xcd"
#define PCODE_FramesLostR2Nmb "\xce"
#define PCODE_FramesLostR3Nmb "\xcf"
#define PCODE_FramesLostR4Nmb "\xd0"
#define PCODE_FramesLostR5Nmb "\xd1"
#define PCODE_PlayErrorInfo "\xd2"
#define PCODE_LogMsgEnable "\xd3"
#define PCODE_LogMsgOrFile "\xd4"
#define PCODE_LogMsgLogFtpServer "\xd5"
#define PCODE_LogMsgLogFtpUser "\xd6"
#define PCODE_LogMsgLogFtpPassword "\xd7"
#define PCODE_LogMsgDuration "\xd8"
#define PCODE_LogMsgRTSPInfo "\xd9"
#define PCODE_LogMsgHTTPInfo "\xda"
#define PCODE_LogMsgIGMPInfo "\xdb"
#define PCODE_LogMsgPkgTotalOneSec "\xdc"
#define PCODE_LogMsgByteTotalOneSec "\xdd"
#define PCODE_LogMsgPkgLostRate "\xde"
#define PCODE_LogMsgAvarageRate "\xdf"
#define PCODE_LogMsgBUFFER "\xe0"
#define PCODE_LogMsgERROR "\xe1"
#define PCODE_LogMsgVendorExt "\xe2"
#define PCODE_UDPConnectionRequestAddress "\xe3"
#define PCODE_UDPConnectionRequestNtfLimit "\xe4"
#define PCODE_STUNEnable "\xe5"
#define PCODE_STUNServerAddress "\xe6"
#define PCODE_STUNServerPort "\xe7"
#define PCODE_STUNUsername "\xe8"
#define PCODE_STUNPassword "\xe9"
#define PCODE_STUNMaxKeepAlivePeriod "\xea"
#define PCODE_STUNMinKeepAlivePeriod "\xeb"
#define PCODE_NATDetected "\xec"
#define PCODE_OperatorInfo "\xed"
#define PCODE_IntegrityCheck "\xee"
#define PCODE_UpgradeURL "\xef"
#define PCODE_BrowserURL2 "\xf0"
#define PCODE_BrowserTagURL "\xf1"
#define PCODE_AdministratorPassword "\xf2"
#define PCODE_CUserPassword "\xf3"
#define PCODE_UserProvince "\xf4"
#define PCODE_PhyMemSize "\xf5"
#define PCODE_StorageSize "\xf6"
#define PCODE_AlarmReason "\xf7"
#define PCODE_AlarmPacketsLostRate "\xf8"
#define PCODE_AlarmFramesLost "\xf9"
#define PCODE_TvmsgResServer "\xfa"
#define PCODE_TvmsgResInterval "\xfb"
#define PCODE_SpeedTestUniqueNum "\xfc"

/* tr069 parameter index */
/// \deprecated
enum {
	TR069_PARAM_Manufacturer = 0,
	TR069_PARAM_ManufacturerOUI,
	TR069_PARAM_ProductClass,
	TR069_PARAM_SerialNumber,
	TR069_PARAM_HardwareVersion,
	TR069_PARAM_SoftwareVersion,
	TR069_PARAM_ProvisioningCode,
	TR069_PARAM_DeviceStatus,
	TR069_PARAM_KernelVersion,
	TR069_PARAM_FsVersion,
	TR069_PARAM_AppVersion,
	TR069_PARAM_StreamingControlProtocols,
	TR069_PARAM_StreamingTransportProtocols,
	TR069_PARAM_StreamingTransportControlProtocols,
	TR069_PARAM_DownloadTransportProtocols,
	TR069_PARAM_MultiplexTypes,
	TR069_PARAM_MaxDejitteringBufferSize,
	TR069_PARAM_AudioStandards,
	TR069_PARAM_VideoStandards,
	TR069_PARAM_ManageSerURL,
	TR069_PARAM_ManageSerUsername,
	TR069_PARAM_ManageSerPassword,
	TR069_PARAM_ConnectionRequestURL,
	TR069_PARAM_ConnectionRequestUsername,
	TR069_PARAM_ConnectionRequestPassword,
	TR069_PARAM_PeriodicInformEnable,
	TR069_PARAM_PeriodicInformInterval,
	TR069_PARAM_PeriodicInformTime,
	TR069_PARAM_UpgradesManaged,
	TR069_PARAM_ParameterKey,
	TR069_PARAM_ManageSerURLBackup,
	TR069_PARAM_NTPServer1,
	TR069_PARAM_LocalTimeZone,
	TR069_PARAM_CurrentLocalTime,
	TR069_PARAM_Device,
	TR069_PARAM_DhcpUser,
	TR069_PARAM_DhcpPassword,
	TR069_PARAM_AddressingType,
	TR069_PARAM_IPAddressReal,
	TR069_PARAM_SubnetMaskReal,
	TR069_PARAM_DefaultGatewayReal,
	TR069_PARAM_DNSServer0Real,
	TR069_PARAM_DNSServer1Real,
	TR069_PARAM_MACAddress,
	TR069_PARAM_ConnectionUpTime,
	TR069_PARAM_TotalBytesSent,
	TR069_PARAM_TotalBytesReceived,
	TR069_PARAM_TotalPacketsSent,
	TR069_PARAM_TotalPacketsReceived,
	TR069_PARAM_CurrentDayInterval,
	TR069_PARAM_CurrentDayBytesSent,
	TR069_PARAM_CurrentDayBytesReceived,
	TR069_PARAM_CurrentDayPacketsSent,
	TR069_PARAM_CurrentDayPacketsReceived,
	TR069_PARAM_QuarterHourInterval,
	TR069_PARAM_QuarterHourBytesSent,
	TR069_PARAM_QuarterHourBytesReceived,
	TR069_PARAM_QuarterHourPacketsSent,
	TR069_PARAM_QuarterHourPacketsReceived,
	TR069_PARAM_PingDiagState,
	TR069_PARAM_PingDiagHost,
	TR069_PARAM_PingDiagReptNumber,
	TR069_PARAM_PingDiagTimeout,
	TR069_PARAM_PingDiagDataBlockSize,
	TR069_PARAM_PingDiagDSCP,
	TR069_PARAM_PingDiagSuccessCount,
	TR069_PARAM_PingDiagFailureCount,
	TR069_PARAM_PingDiagAvgRespTime,
	TR069_PARAM_PingDiagMinRespTime,
	TR069_PARAM_PingDiagMaxRespTime,
	TR069_PARAM_TracertDiagState,
	TR069_PARAM_TracertDiagHost,
	TR069_PARAM_TracertDiagTimeout,
	TR069_PARAM_TracertDiagDataBlockSize,
	TR069_PARAM_TracertDiagMaxHopCount,
	TR069_PARAM_TracertDiagDSCP,
	TR069_PARAM_TracertDiagRespTime,
	TR069_PARAM_TracertDiagHopsNumber,
	TR069_PARAM_HOPHOST1,
	TR069_PARAM_HOPHOST2,
	TR069_PARAM_HOPHOST3,
	TR069_PARAM_HOPHOST4,
	TR069_PARAM_HOPHOST5,
	TR069_PARAM_HOPHOST6,
	TR069_PARAM_HOPHOST7,
	TR069_PARAM_HOPHOST8,
	TR069_PARAM_HOPHOST9,
	TR069_PARAM_HOPHOST10,
	TR069_PARAM_HOPHOST11,
	TR069_PARAM_HOPHOST12,
	TR069_PARAM_HOPHOST13,
	TR069_PARAM_HOPHOST14,
	TR069_PARAM_HOPHOST15,
	TR069_PARAM_HOPHOST16,
	TR069_PARAM_HOPHOST17,
	TR069_PARAM_HOPHOST18,
	TR069_PARAM_HOPHOST19,
	TR069_PARAM_HOPHOST20,
	TR069_PARAM_HOPHOST21,
	TR069_PARAM_HOPHOST22,
	TR069_PARAM_HOPHOST23,
	TR069_PARAM_HOPHOST24,
	TR069_PARAM_HOPHOST25,
	TR069_PARAM_HOPHOST26,
	TR069_PARAM_HOPHOST27,
	TR069_PARAM_HOPHOST28,
	TR069_PARAM_HOPHOST29,
	TR069_PARAM_HOPHOST30,
	TR069_PARAM_HOPHOST31,
	TR069_PARAM_HOPHOST32,
	TR069_PARAM_STBID,
	TR069_PARAM_PPPoEID,
	TR069_PARAM_PPPoEPassword,
	TR069_PARAM_UserID,
	TR069_PARAM_UserPassword,
	TR069_PARAM_AuthURL,
	TR069_PARAM_LogServerUrl,
	TR069_PARAM_LogUploadInterval,
	TR069_PARAM_LogRecordInterval,
	TR069_PARAM_StatInterval,
	TR069_PARAM_PacketsLostR1,
	TR069_PARAM_PacketsLostR2,
	TR069_PARAM_PacketsLostR3,
	TR069_PARAM_PacketsLostR4,
	TR069_PARAM_PacketsLostR5,
	TR069_PARAM_BitRateR1,
	TR069_PARAM_BitRateR2,
	TR069_PARAM_BitRateR3,
	TR069_PARAM_BitRateR4,
	TR069_PARAM_BitRateR5,
	TR069_PARAM_FramesLostR1,
	TR069_PARAM_FramesLostR2,
	TR069_PARAM_FramesLostR3,
	TR069_PARAM_FramesLostR4,
	TR069_PARAM_FramesLostR5,
	TR069_PARAM_Startpoint,
	TR069_PARAM_Endpoint,
	TR069_PARAM_AuthNumbers,
	TR069_PARAM_AuthFailNumbers,
	TR069_PARAM_AuthFailInfo,
	TR069_PARAM_MultiReqNumbers,
	TR069_PARAM_MultiFailNumbers,
	TR069_PARAM_MultiFailInfo,
	TR069_PARAM_VodReqNumbers,
	TR069_PARAM_VodFailNumbers,
	TR069_PARAM_VodFailInfo,
	TR069_PARAM_HTTPReqNumbers,
	TR069_PARAM_HTTPFailNumbers,
	TR069_PARAM_HTTPFailInfo,
	TR069_PARAM_MutiAbendNumbers,
	TR069_PARAM_VODAbendNumbers,
	TR069_PARAM_PlayErrorNumbers,
	TR069_PARAM_MultiPacketsLostR1Nmb,
	TR069_PARAM_MultiPacketsLostR2Nmb,
	TR069_PARAM_MultiPacketsLostR3Nmb,
	TR069_PARAM_MultiPacketsLostR4Nmb,
	TR069_PARAM_MultiPacketsLostR5Nmb,
	TR069_PARAM_VODPacketsLostR1Nmb,
	TR069_PARAM_VODPacketsLostR2Nmb,
	TR069_PARAM_VODPacketsLostR3Nmb,
	TR069_PARAM_VODPacketsLostR4Nmb,
	TR069_PARAM_VODPacketsLostR5Nmb,
	TR069_PARAM_MultiBitRateR1Nmb,
	TR069_PARAM_MultiBitRateR2Nmb,
	TR069_PARAM_MultiBitRateR3Nmb,
	TR069_PARAM_MultiBitRateR4Nmb,
	TR069_PARAM_MultiBitRateR5Nmb,
	TR069_PARAM_VODBitRateR1Nmb,
	TR069_PARAM_VODBitRateR2Nmb,
	TR069_PARAM_VODBitRateR3Nmb,
	TR069_PARAM_VODBitRateR4Nmb,
	TR069_PARAM_VODBitRateR5Nmb,
	TR069_PARAM_FramesLostR1Nmb,
	TR069_PARAM_FramesLostR2Nmb,
	TR069_PARAM_FramesLostR3Nmb,
	TR069_PARAM_FramesLostR4Nmb,
	TR069_PARAM_FramesLostR5Nmb,
	TR069_PARAM_PlayErrorInfo,
	TR069_PARAM_LogMsgEnable,
	TR069_PARAM_LogMsgOrFile,
	TR069_PARAM_LogMsgFtpServer,
	TR069_PARAM_LogMsgFtpUser,
	TR069_PARAM_LogMsgFtpPassword,
	TR069_PARAM_LogMsgDuration,
	TR069_PARAM_LogMsgRTSPInfo,
	TR069_PARAM_LogMsgHTTPInfo,
	TR069_PARAM_LogMsgIGMPInfo,
	TR069_PARAM_LogMsgPkgTotalOneSec,
	TR069_PARAM_LogMsgByteTotalOneSec,
	TR069_PARAM_LogMsgPkgLostRate,
	TR069_PARAM_LogMsgAvarageRate,
	TR069_PARAM_LogMsgBUFFER,
	TR069_PARAM_LogMsgERROR,
	TR069_PARAM_LogMsgVendorExt,
	TR069_PARAM_UDPConnectionRequestAddress,
	TR069_PARAM_UDPConnectionRequestNtfLimit,
	TR069_PARAM_STUNEnable,
	TR069_PARAM_STUNServerAddress,
	TR069_PARAM_STUNServerPort,
	TR069_PARAM_STUNUsername,
	TR069_PARAM_STUNPassword,
	TR069_PARAM_STUNMaxKeepAlivePeriod,
	TR069_PARAM_STUNMinKeepAlivePeriod,
	TR069_PARAM_NATDetected,
	TR069_PARAM_OperatorInfo,
	TR069_PARAM_IntegrityCheck,
	TR069_PARAM_UpgradeURL,
	TR069_PARAM_BrowserURL2,
	TR069_PARAM_BrowserTagURL,
	TR069_PARAM_AdministratorPassword,
	TR069_PARAM_CUserPassword,
	TR069_PARAM_UserProvince,
	TR069_PARAM_PhyMemSize,
	TR069_PARAM_StorageSize,
	TR069_PARAM_AlarmReason,
	TR069_PARAM_AlarmPacketsLostRate,
	TR069_PARAM_AlarmFramesLost,
	TR069_PARAM_TvmsgResServer,
	TR069_PARAM_TvmsgResInterval,
	TR069_PARAM_SpeedTestUniqueNum,
	TR069_PARAM_SpeedTestEnable,
	TR069_PARAM_SpeedTestDuration,
	TR069_PARAM_SpeedTestPath,
	TR069_PARAM_SpeedTestCode,
	TR069_PARAM_SpeedTestResult,
	TR069_PARAM_MultiRRT,
	TR069_PARAM_VodRRT,
	TR069_PARAM_HTTPRRT,
	TR069_PARAM_MultiAbendNumbers,
	TR069_PARAM_MultiAbendUPNumbers,
	TR069_PARAM_VODUPAbendNumbers,
	TR069_PARAM_FECMultiPacketsLostR1Nmb,
	TR069_PARAM_FECMultiPacketsLostR2Nmb,
	TR069_PARAM_FECMultiPacketsLostR3Nmb,
	TR069_PARAM_FECMultiPacketsLostR4Nmb,
	TR069_PARAM_FECMultiPacketsLostR5Nmb,
	TR069_PARAM_ARQVODPacketsLostR1Nmb,
	TR069_PARAM_ARQVODPacketsLostR2Nmb,
	TR069_PARAM_ARQVODPacketsLostR3Nmb,
	TR069_PARAM_ARQVODPacketsLostR4Nmb,
	TR069_PARAM_ARQVODPacketsLostR5Nmb,
	TR069_PARAM_BufferDecNmb,
	TR069_PARAM_BufferIncNmb,
	TR069_PARAM_SQMEnableEPG,
	TR069_PARAM_SQMEnableMedia,
	TR069_PARAM_SQMEnableMediaBeforeEC,
	TR069_PARAM_SQMEnableWarning,
	TR069_PARAM_SQMEnableTelnet,
	TR069_PARAM_SQMMediaInterval,
	TR069_PARAM_SQMServerAddress,
	TR069_PARAM_SQMServerPort,
	TR069_PARAM_AuthURLBackup,
	TR069_PARAM_ModelName,
	TR069_PARAM_ModelID,
	TR069_PARAM_Description,
	TR069_PARAM_ConfigFileVersion,
	TR069_PARAM_UpTime,
	TR069_PARAM_FirstUseDate,
	TR069_PARAM_DeviceLog,
	TR069_PARAM_ConfigFile,
	TR069_PARAM_CurrentLanguage,
	TR069_PARAM_AvailableLanguages,
	TR069_PARAM_IsSupportIPv6,
	TR069_PARAM_PacketLostTestEnable,
	TR069_PARAM_PacketLostTestUDPPort,
	TR069_PARAM_PacketLostTestBand,
	TR069_PARAM_AuthServiceInfoPPPOEEnable,
	TR069_PARAM_AuthServiceInfoPPPOEID2,
	TR069_PARAM_AuthServiceInfoPPPOEPassword2,
	TR069_PARAM_AuthServiceInfoUserID2,
	TR069_PARAM_AuthServiceInfoUserIDPassword2,
	TR069_PARAM_NUM
};

extern int tr069_index_map[TR069_PARAM_NUM];

#define TR069_HEARTBEAT_LIMIT 60
#define TR069_LOGMSG_LIMIT 6000

#define TR069_STAND_CU 1

#define TR069_STAND_CTC 0

/* inform event types */
typedef enum
{
	/* 0 BOOTSTRAP */
	TR069_EVENT_BOOTSTRAP = 0,
	/* 1 BOOT */
	TR069_EVENT_BOOT = 1,
	/* 2 PERIODIC */
	TR069_EVENT_PERIODIC = 2,
	/* 3 SCHEDULED */
	TR069_EVENT_SCHEDULED = 3,
	/* 4 VALUE CHANGE */
	TR069_EVENT_VALUE_CHANGE = 4,
	/* 5 KICKED */
	TR069_EVENT_KICKED = 5,
	/* 6 CONNECTION REQUEST */
	TR069_EVENT_CONNECTION_REQUEST = 6,
	/* 7 TRANSFER COMPLETE */
	TR069_EVENT_TRANSFER_COMPLETE = 7,
	/* 8 DIAGNOSTICS COMPLETE */
	TR069_EVENT_DIAGNOSTICS_COMPLETE = 8,
	/* 9 REQUEST DOWNLOAD */
	TR069_EVENT_REQUEST_DOWNLOAD = 9,
	/* 10 AUTONOMOUS TRANSFER COMPLETE */
	TR069_EVENT_AUTONMOUS_TRANSFER_COMPLETE = 10,
	/* 11 X CU ALARM */
	TR069_EVENT_X_CU_ALARM = 11,
	/* M ScheduleInform */
	TR069_EVENT_M_SCHEDULEINFORM = 12,
	/* M Reboot */
	TR069_EVENT_M_REBOOT = 13,
	/* M Download */
	TR069_EVENT_M_DOWNLOAD = 14,
	/* M Upload */
	TR069_EVENT_M_UPLOAD = 15,
	/* M CTC LOG_PERIODIC */
	TR069_EVENT_M_CTC_LOG_PERIODIC = 16,

	TR069_EVENT_NUMBER
}tr069_event_type;

#define TR069_DEFAULT_EVENT_FLAG "00000000000000000"

/* fault types */
typedef enum
{
	TR069_FAULT_NO_FAULT = 0,

	/* acs standard fault */
	TR069_FAULT_ACS_METHOD_NOT_SUPPORTED = 8000,
	TR069_FAULT_ACS_REQUEST_DENIED,
	TR069_FAULT_ACS_INTERNAL_ERROR,
	TR069_FAULT_ACS_INVALID_ARGUMENTS,
	TR069_FAULT_ACS_RESOURCES_EXCEEDED,
	TR069_FAULT_ACS_RETRY_REQUEST,
	/* acs custom fault */
	TR069_FAULT_ACS_VENDOR_DEFINED = 8800,

	/* cpe standard fault */
	TR069_FAULT_CPE_METHOD_NOT_SUPPORTED = 9000,
	TR069_FAULT_CPE_REQUEST_DENIED,
	TR069_FAULT_CPE_INTERNAL_ERROR,
	TR069_FAULT_CPE_INVALID_ARGUMENTS,
	TR069_FAULT_CPE_RESOURCES_EXCEEDED,
	TR069_FAULT_CPE_INVALID_PARAMETER_NAME,
	TR069_FAULT_CPE_INVALID_PARAMETER_TYPE,
	TR069_FAULT_CPE_INVALID_PARAMETER_VALUE,
	TR069_FAULT_CPE_PARAMETER_NOT_WRITABLE,
	TR069_FAULT_CPE_NOTIFICATION_REQUEST_REJECTED,
	TR069_FAULT_CPE_DOWNLOAD_FAILURE,
	TR069_FAULT_CPE_UPLOAD_FAILURE,
	/* cpe custom fault */
	TR069_FAULT_CPE_VENDOR_DEFINED = 9800
}tr069_fault_type;

/* parameter data types */
typedef enum
{
	/* object for partial path parameter */
	TR069_DATATYPE_OBJECT = 0,
	/* most common data type */
	TR069_DATATYPE_STRING,
	/* int */
	TR069_DATATYPE_INT,
	/* unsigned int */
	TR069_DATATYPE_UNSIGNEDINT,
	/* boolean */
	TR069_DATATYPE_BOOLEAN,
	/* utc time format at YYYY-MM-DDTHH:MM:SS
	   use 0001-01-01T00:00:00Z as "invalid time" or "unknown time"
	   time has YYYY lower than 1000 should stand for relative time from BOOT
	   absolute time must have time zone suffix */
	TR069_DATATYPE_DATETIME,
	/* binary data coded with base64 */
	TR069_DATATYPE_BASE64,

	TR069_DATATYPE_NUMBER
}tr069_data_type;

/* notification types */
typedef enum
{
	/* do not notify acs when value change */
	TR069_NOTIFICATION_OFF = 0,
	/* notify acs by next inform when value change */
	TR069_NOTIFICATION_PASSIVE,
	/* notify acs just when value change */
	TR069_NOTIFICATION_ACTIVE
}tr069_notification_type;

/* rpc types
   comment rpcs not realized yet */
typedef enum
{
	/* empty http post or response */
	TR069_RPC_EMPTY = 0,

	TR069_RPC_ACS_INFORM,
	TR069_RPC_ACS_TRANSFER_COMPLETE,
	/*
	TR069_RPC_ACS_GET_RPC_METHODS,
	TR069_RPC_ACS_KICKED,
	*/

	TR069_RPC_CPE_SET_PARAMETER_VALUES,
	TR069_RPC_CPE_GET_PARAMETER_VALUES,
	TR069_RPC_CPE_GET_PARAMETER_NAMES,
	TR069_RPC_CPE_DOWNLOAD,
	TR069_RPC_CPE_UPLOAD,
	TR069_RPC_CPE_REBOOT,
	TR069_RPC_CPE_FACTORY_RESET,
	/*
	TR069_RPC_CPE_GET_RPC_METHODS,
	*/
	TR069_RPC_CPE_SET_PARAMETER_ATTRIBUTES,
	TR069_RPC_CPE_GET_PARAMETER_ATTRIBUTES,

	TR069_RPC_CPE_X_FV_SETUP,

	TR069_RPC_NUMBER
}tr069_rpc_type;

/* file types */
typedef enum
{
	/* 1 Firmware Upgrade Image */
	TR069_DOWNLOAD_FILE_UPGRADE_IMAGE,
	/* 2 Web Content */
	TR069_DOWNLOAD_FILE_WEB_CONTENT,
	/* 3 Vendor Configuration File */
	TR069_DOWNLOAD_FILE_VENDOR_CONFIG,
	/* 4 Vendor Log File */
	TR069_DOWNLOAD_FILE_VENDOR_LOG,
	/* 1 Vendor Configuration File */
	TR069_UPLOAD_FILE_VENDOR_CONFIG,
	/* 2 Vendor Log File */
	TR069_UPLOAD_FILE_VENDOR_LOG
}tr069_file_type;

/* work status types of tr069 manager */
typedef enum
{
	/* before work, environment parameters modification applied to ini file */
	TR069_STATUS_BEFORE_WORK = 0,
	/* loadding environment, environment parameters modification disable */
	TR069_STATUS_LOAD_ENV,
	/* in work, environment parameters modification applied to memory */
	TR069_STATUS_WORKING,
	/* after work, environment parameters modification applied to ini file */
	TR069_STATUS_AFTER_WORK
}tr069_status;

/* tr069 manager representing tr069 model */
typedef struct _tr069_mgr
{
	/* handle to control manager */
	void *control_handle;
	/* handle to tr111 manager */
	void *tr111_handle;
	/* handle to parameter manager */
	void *parameter_handle;
	/* handle to rpc manager */
	void *rpc_handle;
	/* handle to session manager */
	void *session_handle;
	/* handle to interface */
	void *interface_handle;

	/* work status of tr069 manager */
	tr069_status status;
	/* semaphore to avoid config edit collision */
	sem_t config_sem;
	/* number of processes calling tr069 */
	int call_flag;
}tr069_mgr;

int n_tr069_start();
int n_tr069_stop();
int n_tr069_set_event(int event_index,unsigned char event_value,char * command_key);
int n_tr069_put_request(int request );
int n_tr069_put_inform_param(char *param_name);
int n_tr069_set_transfer_complete(char *start_time,char *complete_time,int transfer_result );
int n_tr069_cu_usb_config();
int n_tr069_get_firstboot_flag(char *value, int size);
int n_tr069_set_firstboot_flag(char *value);
int n_tr069_offline_notify();
int n_tr069_value_change_notify(char *param);
int n_tr069_alarm_report();

#ifdef __cplusplus
}
#endif

#endif
