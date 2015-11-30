#ifndef O_FLASH_HG650O_H
#define O_FLASH_HG650O_H

typedef enum {
	O_FLASH_PART_APP = 0,
	O_FLASH_PART_INITRD,
	O_FLASH_PART_KERNEL,
	O_FLASH_PART_PARAM,
	O_FLASH_PART_PARTINFO,
	O_FLASH_PART_LOGO,
	O_FLASH_PART_UBOOT,
	O_FLASH_PART_MAX
} o_flash_part_e;

#define O_FLASH_BLOCK_SIZE            (128 * 1024) //block size
#define O_FLASH_MAX_LOGO_SIZE 	      (1536*1024)

#define O_FLASH_IMGFILE_PATH          "/var/uptmp/"
#define O_FLASH_IMAGE_HEAD_VER        "mtk8652.0.0"

#define UPGRADE_BLOCK_PART_UBOOT      "/dev/mtd1"
#define UPGRADE_BLOCK_PART_INFO1      "/dev/mtd2"
#define UPGRADE_BLOCK_PART_INFO2      "/dev/mtd3"
#define UPGRADE_BLOCK_PART_KERNEL1    "/dev/mtd13"
#define UPGRADE_BLOCK_PART_KERNEL2    "/dev/mtd14"
#define UPGRADE_BLOCK_PART_APP1       "/dev/mtd16"
#define UPGRADE_BLOCK_PART_APP2       "/dev/mtd18"
#define UPGRADE_BLOCK_PART_INITRD1    "/dev/mtd15"
#define UPGRADE_BLOCK_PART_INITRD2    "/dev/mtd17"
#define UPGRADE_BLOCK_PART_LOGO       "/dev/mtd20"
#define UPGRADE_BLOCK_PART_PARAM      "/dev/mtd23"


#define O_FLASH_BLOCK_ID_UBOOT        1
#define O_FLASH_BLOCK_ID_PIT1         2
#define O_FLASH_BLOCK_ID_PIT2         3
#define O_FLASH_BLOCK_ID_UPG_STATUS   19
#define O_FLASH_BLOCK_ID_APP1         16
#define O_FLASH_BLOCK_ID_APP2         18
#define O_FLASH_BLOCK_ID_KERNEL1      13
#define O_FLASH_BLOCK_ID_KERNEL2      14
#define O_FLASH_BLOCK_ID_INITRD1      15
#define O_FLASH_BLOCK_ID_INITRD2      17
#define O_FLASH_BLOCK_ID_LOGO         20
#define O_FLASH_BLOCK_ID_PARAM        23

#define O_FLASH_LOGO_PARTITION       UPGRADE_BLOCK_PART_LOGO

#define O_FLASH_BIN_NAME_APP          "rootfs_normal.img"
#define O_FLASH_BIN_NAME_INITRD       "initrd"
#define O_FLASH_BIN_NAME_KERNEL       "uImage"
#define O_FLASH_BIN_NAME_PARAM        "ubi_param.bin"
#define O_FLASH_BIN_NAME_PARTINFO     "part_info.bin"
#define O_FLASH_BIN_NAME_LOGO         "booting.png"
#define O_FLASH_BIN_NAME_UBOOT        "mt8652_bl_linux.bin"

int flash_erase_partition(void *param1, void *param2);
int flash_write_partition(void *param1, void *param2, void *param3, void *param4);

#endif
