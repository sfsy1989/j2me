#ifndef __F_STB_MP__
#define __F_STB_MP__

enum {
	MP_CFG_FramebufferWidth = 0,
	MP_CFG_FramebufferHeight,
	MP_CFG_MP_Volume,
	MP_CFG_VideoMode,
	MP_CFG_PacketsLostR1,
	MP_CFG_PacketsLostR2,
	MP_CFG_PacketsLostR3,
	MP_CFG_PacketsLostR4,
	MP_CFG_PacketsLostR5,
	MP_CFG_BitRateR1,
	MP_CFG_BitRateR2,
	MP_CFG_BitRateR3,
	MP_CFG_BitRateR4,
	MP_CFG_BitRateR5,
	MP_CFG_FramesLostR1,
	MP_CFG_FramesLostR2,
	MP_CFG_FramesLostR3,
	MP_CFG_FramesLostR4, 
	MP_CFG_FramesLostR5,
	MP_CFG_LogRecordInterval,  
	MP_CFG_StatInterval,
	MP_CFG_STBID,
	MP_CFG_NUM,
};

#define MP_CFG_FIRST MP_CFG_FramebufferWidth

int n_mp_get_config(int key, char *value, int size);

#endif

