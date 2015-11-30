#ifndef __O_SOC_CAP_H__
#define __O_SOC_CAP_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	O_SC_HD = 0,
	O_SC_SWITCH,
	O_SC_MCU,
	O_SC_MULTIPLE_OUTPUT,
	O_SC_MULTIPLE_OUTPUT_DIFF_RESO,
	O_SC_OSD_SPLASH,
	O_SC_OSD_SPLASH_DELAY,
	O_SC_UPG_LARGE,
	O_SC_PHYMEMSIZE,
	O_SC_STORAGE_SIZE
} o_soc_cap_e;

int o_soc_cap_query(o_soc_cap_e cap, int *pval);
int o_soc_cap_query_string(o_soc_cap_e cap, char *sval, int len);

#ifdef __cplusplus
}
#endif

#endif

