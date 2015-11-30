#ifndef __F_DUMP_H__
#define __F_DUMP_H__

#include <sys/types.h>

#define SHM_DIR "/data/iptv/"
#define SHM_PREFIX "flib_shm"
#define SHM_OPEN_FLAG O_RDWR | O_CREAT
#define SHM_OPEN_MODE 0666

void *f_get_dump_addr(int  id, size_t size);
int f_dump_rel(void *addr, size_t size);
int f_dump_del(int id);

#endif
