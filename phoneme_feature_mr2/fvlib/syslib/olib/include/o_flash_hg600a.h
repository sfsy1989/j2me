#ifndef __O_FLASH_HG600A_H__
#define __O_FLASH_HG600A_H__

typedef enum {
	O_FLASH_PART_APP,
	O_FLASH_PART_KERNEL,
	O_FLASH_PART_PARAM,
	O_FLASH_PART_LOGO,
	O_FLASH_PART_MAX
} o_flash_part_e;

#define O_FLASH_IMGFILE_PATH	        "/var/uptmp/"
#define O_FLASH_BLOCK_KERNEL1           "/dev/mtd/1"
#define O_FLASH_BLOCK_KERNEL2           "/dev/mtd/2"
#define O_FLASH_BLOCK_BASICFS1          "/dev/mtd/3"
#define O_FLASH_BLOCK_BASICFS2          "/dev/mtd/4"
#define O_FLASH_BLOCK_LOGO1             "/dev/mtd/5"
#define O_FLASH_BLOCK_LOGO2             "/dev/mtd/6"

#define O_FLASH_IMAGE_HEAD_VER          "hi3560e.0.0"

#define O_FLASH_LOGO_ADDR1              "0x07400000"
#define O_FLASH_LOGO_ADDR2              "0x07500000"

#define O_FLASH_LOGO_PARTITION          O_FLASH_BLOCK_LOGO1

#define O_FLASH_BIN_NAME_APP            "basicfs.y2"
#define O_FLASH_BIN_NAME_KERNEL         "hikernel"
#define O_FLASH_BIN_NAME_PARAM          "param.y2"
#define O_FLASH_BIN_NAME_LOGO           "booting.jpg"

int dd_erase_partition(void *param1, void *param2);
int dd_write_partition(
	void *param1,
	void *param2,
	void *param3,
	void *param4);
int yaffs_erase_partition(void *param1, void *param2);
int yaffs_write_partition(
	void *param1,
	void *param2,
	void *param3,
	void *param4);

#endif
