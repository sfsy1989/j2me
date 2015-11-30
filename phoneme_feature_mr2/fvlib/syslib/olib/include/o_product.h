#ifndef __O_PRODUCT_H__
#define __O_PRODUCT_H__
#ifdef __cplusplus
extern "C" {
#endif

enum {
	O_PRODUCT_HI_3560E = 0,
	O_PRODUCT_SIGMA_8653,
	O_PRODUCT_SIGMA_8670,
	O_PRODUCT_MTK_8652,
	O_PRODUCT_MTK_8550,
        O_PRODUCT_ANDROID,
};

#ifndef ANDROID
int o_product_get_type();
#else
static inline int o_product_get_type()
{
	return O_PRODUCT_ANDROID;
}
#endif

#ifdef __cplusplus
}
#endif

#endif

