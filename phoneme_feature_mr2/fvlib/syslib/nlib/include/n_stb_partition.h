#ifndef _F_PARTITION_H_
#define _F_PARTITION_H_

/************************************************************************************************/
//BE  CAREFUL TO  MODIFY  THIS  FILE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/************************************************************************************************/
#define IMG_NAME_MAX_LEN   0x20
#define IMG_MAX_NUM        0x20
#define UPGRADE_DESC_NAME  "u_up_v1.desc"
#define UBOOT_UP_VERSION   "u_up_version"
#define UBOOT_UPGRADE_FLAG "upgflag"

typedef enum {
	SKIP = 0,
	ERASE,
	ERASE_WRITE
} fl_op;

typedef struct {
        char imgname[IMG_NAME_MAX_LEN];
        fl_op operation;
        unsigned char imgtype;  /*0 u-boot;1 kernel;2 yaffs2;3.eeprom;4.standalone */
        unsigned int st;        /*st is the abs address */
        unsigned int nd;        /*nd is size of partition,not the abs address */
} upgrade_img;

typedef struct {
        int img_n;
        int version;            //0  must upgrade ,1-n version control upgrade.
        upgrade_img imgs[IMG_MAX_NUM];
} upgrade_desc;

#define PARTITION_NUM  11



#define FV_UBOOT  0
#define FV_KERNEL  1
#define FV_YAFFS2 2
#define FV_EEPROM  3
#define FV_STANDALONE  4
#define kernelname "hikernel.hh"
#define basicfsname "basicfs.y2.hh"
#define urgentfsname "urgentfs.y2.hh"
#define bootingjpgname "booting.jpg.hh"
#define eepromname     "eeprom.bin.hh"
#define ubootname      "u-boot.bin.hh"
#define paramname    "param.y2.hh"


/*gcc complains redefinition of fv_partitions and partition_name in uboot code!*/
#ifndef    FV_CODE_IN_UBOOT
/*partition table*/
upgrade_img fv_partitions[PARTITION_NUM] = {
        {kernelname,     0, FV_KERNEL,     0x00200000, 0x00200000}, /*kernel_1*/
        {kernelname,     0, FV_KERNEL,     0x00400000, 0x00200000}, /*kernel_2*/
        {bootingjpgname, 0, FV_STANDALONE, 0x07400000, 0x00100000}, /*logo_1*/
        {bootingjpgname, 0, FV_STANDALONE, 0x07500000, 0x00100000}, /*logo_2*/
        {basicfsname,    0, FV_YAFFS2,     0x00600000, 0x03700000}, /*basicfs_1*/
        {basicfsname,    0, FV_YAFFS2,     0x03d00000, 0x03700000}, /*basicfs_2*/
        {kernelname,     0, FV_KERNEL,     0,          0x0},        /*urgentkernel*/
        {urgentfsname,   0, FV_YAFFS2,     0,          0x0},        /*urgentfs*/
        {eepromname,     0, FV_EEPROM,     0,          0},          /*eeprom*/
        {paramname,      0, FV_YAFFS2,     0x07600000, 0x00A00000}, /*paras*/
        {ubootname,      0, FV_UBOOT,      0x00000000, 0x00200000}  /*uboot*/
};


/*for debug */
char partition_name[PARTITION_NUM][20]=
{
	"kernel_1",
	"kernel_2",
	"logo_1",
	"logo_2",
	"basicfs_1",
	"basicfs_2",
	"urgentkernel",
	"urgentfs",
	"eeprom",
	"paras",
	"uboot"
};

#endif

/*************************************************************************************************/

#define FV_BOOTCMD_KERNEL1   "nand read c1300000 200000 130000;bootm c1300000"
#define FV_BOOTCMD_KERNEL2   "nand read c1300000 400000 130000;bootm c1300000"
#define FV_BOOTCMD_URGENT_KERNEL   "nand read c1300000 6800000 130000;bootm c1300000"

#define FV_BOOTARGS_FS1       "mem=98M console=ttyAMA0,115200 root=/dev/mtdblock/3 rootfstype=yaffs2 ro mtdparts=hinand:2M(bt),2M(k1),2M(k2),55M(bs1),55M(bs2),1M(l1),1M(l2),10M(ps)"
#define FV_BOOTARGS_FS2       "mem=98M console=ttyAMA0,115200 root=/dev/mtdblock/4 rootfstype=yaffs2 ro mtdparts=hinand:2M(bt),2M(k1),2M(k2),55M(bs1),55M(bs2),1M(l1),1M(l2),10M(ps)"
#define FV_BOOTARGS_URGENT_FS "mem=88M console=ttyAMA0,115200 root=/dev/mtdblock/9 rootfstype=yaffs2 ro mtdparts=hinand:2M(bt),4M(k1),4M(k2),42M(bs1),42M(bs2),2M(l1),2M(l2),6M(ps),2M(uk),22M(uf)"

#define FV_LOGOADDR1 "0x07400000"
#define FV_LOGOADDR2 "0x07500000"

#define FV_BOOTARGS "fv_bootargs"
#define FV_BOOTCMD  "fv_bootcmd"

/************************************************************************************************/
//BE  CAREFUL TO  MODIFY  THIS  FILE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/************************************************************************************************/

#endif
