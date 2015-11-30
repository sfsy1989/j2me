#ifndef __F_FLASH_H__
#define __F_FLASH_H__

int f_flash_erase(char *mtd_device, int offset, int length);
int f_flash_eraseall(char *mtd_device);
int f_flash_nandwrite(char *mtd_device, char *img, int flag, int start_pos, int length);

#endif
