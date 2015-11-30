#ifndef __F_STB_CFG_H__
#define __F_STB_CFG_H__

#define N_CFG_MAX_VALUE_SIZE    4096

#define cmd_cfg_get_config	    "config.GetConfig"
#define cmd_cfg_set_config          "config.SetConfig"
#define cmd_cfg_set_attribute  	    "config.SetAttribute"
#define cmd_cfg_sync_config         "config.SyncConfig"
#define cmd_cfg_sync_tr069	    "config.SyncTr069"
#define cmd_cfg_reload_config       "config.ReloadConfig"
#define cmd_cfg_save_config         "config.SaveConfig"
#define cmd_cfg_factory_reset       "config.FactoryReset"
#define cmd_cfg_save_tr069	    "config.SaveTr069"
#define cmd_cfg_usb_ini_read        "config.usb_ini_read"
#define cmd_cfg_save_iptv           "config.SaveIptv"
#define cmd_cfg_save_stb_info       "config.SaveStbInfo"
#define cmd_cfg_set_burnmac_timmer  "config.SetBurnmacTimmer"
#define cmd_cfg_copy_ini_file       "config.copy_ini_file"

#define FOUND_USB0_INI_FILE 0
#define FOUND_USB1_INI_FILE 1

typedef enum {
	CFG_BURN_MAC_START,
	CFG_BURN_MAC_STOP,
	CFG_BURN_MAC_NONE
}cfg_burn_mac_action;

enum {
	F_CFG_CONFIG_INI_VERSION = 0,
	F_CFG_PRODUCT_INI_VERSION,
	F_CFG_STB_INI_VERSION,
	F_CFG_IPTV_INI_VERSION,
	F_CFG_IPTV_STANDARD,
	F_CFG_OperatorInfo,
	F_CFG_AdministratorPassword,
	F_CFG_CUserPassword,
	F_CFG_UserProvince,
	F_CFG_StreamingControlProtocols,
	F_CFG_StreamingTransportProtocols,
	F_CFG_StreamingTransportControlProtocols,
	F_CFG_DownloadTransportProtocols,
	F_CFG_MultiplexTypes,
	F_CFG_MaxDejitteringBufferSize,
	F_CFG_AudioStandards,
	F_CFG_VideoStandards,
	F_CFG_Manufacturer,
	F_CFG_ManufacturerOUI,
	F_CFG_ProductClass,
	F_CFG_SerialNumber,
	F_CFG_ProvisioningCode,
	F_CFG_DeviceStatus,
	F_CFG_ConfigVersion,
	F_CFG_Type,
	F_CFG_AclUsername,
	F_CFG_AclPassword,
	F_CFG_ManageSerURL,
	F_CFG_ManageSerUsername,
	F_CFG_ManageSerPassword,
	F_CFG_ConnectionRequestURL,
	F_CFG_ConnectionRequestUsername,
	F_CFG_ConnectionRequestPassword,
	F_CFG_PeriodicInformEnable,
	F_CFG_PeriodicInformInterval,
	F_CFG_PeriodicInformTime,
	F_CFG_UpgradesManaged,
	F_CFG_ParameterKey,
	F_CFG_ManageSerURLBackup,
	F_CFG_UDPConnectionRequestAddress,
	F_CFG_UDPConnectionRequestNtfLimit,
	F_CFG_STUNEnable,
	F_CFG_STUNServerAddress,
	F_CFG_STUNServerPort,
	F_CFG_STUNUsername,
	F_CFG_STUNPassword,
	F_CFG_STUNMaxKeepAlivePeriod,
	F_CFG_STUNMinKeepAlivePeriod,
	F_CFG_NTPServer1,
	F_CFG_LocalTimeZone,
	F_CFG_CurrentLocalTime,
	F_CFG_NTPServer2,
	F_CFG_Wireless,
	F_CFG_Device,
	F_CFG_DhcpUser,
	F_CFG_DhcpPassword,
	F_CFG_AddressingType,
	F_CFG_IPAddress,
	F_CFG_IPAddressReal,
	F_CFG_SubnetMask,
	F_CFG_SubnetMaskReal,
	F_CFG_DefaultGateway,
	F_CFG_DefaultGatewayReal,
	F_CFG_DNSServer0,
	F_CFG_DNSServer0Real,
	F_CFG_DNSServer1,
	F_CFG_DNSServer1Real,
	F_CFG_Wireless_SSID,
	F_CFG_Wireless_AUTH,
	F_CFG_Wireless_Enc,
	F_CFG_Wireless_Key,
	F_CFG_PPPoEID,
	F_CFG_PPPoEPassword,
	F_CFG_UserID,
	F_CFG_UserPassword,
	F_CFG_UserGroupNMB,
	F_CFG_AuthURL,
	F_CFG_AuthURLBackup,
	F_CFG_TvmsgResServer,
	F_CFG_TvmsgResInterval,
	F_CFG_LocalUpgradeUrl,
	F_CFG_LocalLogServerUrl,
	F_CFG_LocalManageServerUrl,
	F_CFG_LogServerUrl,
	F_CFG_LogUploadInterval,
	F_CFG_LogRecordInterval,
	F_CFG_StatInterval,
	F_CFG_PacketsLostR1,
	F_CFG_PacketsLostR2,
	F_CFG_PacketsLostR3,
	F_CFG_PacketsLostR4,
	F_CFG_PacketsLostR5,
	F_CFG_BitRateR1,
	F_CFG_BitRateR2,
	F_CFG_BitRateR3,
	F_CFG_BitRateR4,
	F_CFG_BitRateR5,
	F_CFG_FramesLostR1,
	F_CFG_FramesLostR2,
	F_CFG_FramesLostR3,
	F_CFG_FramesLostR4,
	F_CFG_FramesLostR5,
	F_CFG_LogMsgEnable,
	F_CFG_LogMsgOrFile,
	F_CFG_LogMsgFtpServer,
	F_CFG_LogMsgFtpUser,
	F_CFG_LogMsgFtpPassword,
	F_CFG_LogMsgDuration,
	F_CFG_LogMsgRTSPInfo,
	F_CFG_LogMsgHTTPInfo,
	F_CFG_LogMsgIGMPInfo,
	F_CFG_LogMsgPkgTotalOneSec,
	F_CFG_LogMsgByteTotalOneSec,
	F_CFG_LogMsgPkgLostRate,
	F_CFG_LogMsgAvarageRate,
	F_CFG_LogMsgBUFFER,
	F_CFG_LogMsgERROR,
	F_CFG_LogMsgVendorExt,
	F_CFG_UserToken,
	F_CFG_EPGDomain,
	F_CFG_EPGDomainBackup,
	F_CFG_UpgradeDomain,
	F_CFG_UpgradeDomainBackup,
	F_CFG_MP_Volume,
	F_CFG_Language,
	F_CFG_DoubleMode,
	F_CFG_LAN_PORT_MODE,
	F_CFG_DisplayMode,
	F_CFG_Log,
	F_CFG_UpgradeStatus,
	F_CFG_EnableAlert,
	F_CFG_StandbyStatus,
	F_CFG_NetRestartStatus,
	F_CFG_VideoMode,
	F_CFG_KernelVersion,
	F_CFG_FsVersion,
	F_CFG_LogoVersion,
	F_CFG_MACAddress,
	F_CFG_STBID,
	F_CFG_HardwareVersion,
	F_CFG_FramebufferWidth,
	F_CFG_FramebufferHeight,
	F_CFG_HdmiMode,
	F_CFG_IR_TYPE,
	F_CFG_StandbyMode,
	F_CFG_FTMVERSION,
	F_CFG_BUILD_FLAG,
	F_CFG_BuildTime,
	F_CFG_IPV6,
	F_CFG_IPV6Stateless,
	F_CFG_IPortalEnable,
	F_CFG_IPortalName1,
	F_CFG_IPortalValue1,
	F_CFG_IPortalName2,
	F_CFG_IPortalValue2,
	F_CFG_IPortalName3,
	F_CFG_IPortalValue3,
	F_CFG_IPortalName4,
	F_CFG_IPortalValue4,
	F_CFG_SQMEnableEPG,
	F_CFG_SQMEnableMedia,
	F_CFG_SQMEnableMediaBeforeEC,
	F_CFG_SQMEnableWarning,
	F_CFG_SQMEnableTelnet,
	F_CFG_SQMMediaInterval,
	F_CFG_SQMServerAddress,
	F_CFG_SQMServerPort,
	F_CFG_MULTICAST_IP,
	F_CFG_MULTICAST_PORT,
	F_CFG_MULTICAST_SRC_IP,
	F_CFG_TEST_DOMIAN,
	F_CFG_BOOT_LOGO,
	F_CFG_AUTH_LOGO,
	F_CFG_BootLogoVersion,
	F_CFG_AuthLogoVersion,
	F_CFG_NUM
};

int n_cfg_get_config(int key, char *value, int size);
int n_cfg_set_config(int key, char *value);
int n_cfg_set_parameter_attribute(char *config_cmds);
int n_cfg_set_config_int(int key, int value);
int n_cfg_sync_config(int key, char *value, int value_len);
int n_cfg_sync_tr069(int key, char *value, int value_len);
int n_cfg_reload_config(void);
int n_cfg_save_config(void);
int n_cfg_save_tr069(void);
int n_cfg_save_iptv(void);
int n_cfg_save_stb_info(void);
int n_cfg_factory_reset(void);
int n_cfg_usb_ini_read(char *ini_file_name);
int n_cfg_set_burnmac_timmer(cfg_burn_mac_action action);
int n_cfg_set_log_level(char *log_args);
int n_cfg_copy_ini_file(int which_usb_ini_file);

#endif


