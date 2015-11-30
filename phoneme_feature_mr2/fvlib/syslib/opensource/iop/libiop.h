#include <stddef.h>
#include <stdio.h>

#define __ssize_t int
#define _IO_size_t size_t
#define _IO_ssize_t __ssize_t
#define _IO_off_t int
#define _IO_lock_t void
#define _IO_off64_t long long
#define MAYBE_SET_EINVAL /* nothing */
#define COERCE_FILE(FILE) /* Nothing */
#define CHECK_FILE(FILE, RET) COERCE_FILE (FILE)

struct _IO_FILE {
  int _flags;     /* High-order word is _IO_MAGIC; rest is flags. */
#define _IO_file_flags _flags

  /* The following pointers correspond to the C++ streambuf protocol. */
  /* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
  char* _IO_read_ptr;   /* Current read pointer */
  char* _IO_read_end;   /* End of get area. */
  char* _IO_read_base;  /* Start of putback+get area. */
  char* _IO_write_base; /* Start of put area. */
  char* _IO_write_ptr;  /* Current put pointer. */
  char* _IO_write_end;  /* End of put area. */
  char* _IO_buf_base;   /* Start of reserve area. */
  char* _IO_buf_end; /* End of reserve area. */
  /* The following fields are used to support backing up and undo. */
  char *_IO_save_base; /* Pointer to start of non-current get area. */
  char *_IO_backup_base;  /* Pointer to first valid character of backup area */
  char *_IO_save_end; /* Pointer to end of non-current get area. */

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
#if 0
  int _blksize;
#else
  int _flags2;
#endif
  _IO_off_t _old_offset; /* This used to be _offset but it's too small.  */

#define __HAVE_COLUMN /* temporary */
  /* 1+column number of pbase(); 0 is unknown. */
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];

  /*  char* _save_gptr;  char* _save_egptr; */

  _IO_lock_t *_lock;
  _IO_off64_t _offset;
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;
  int _mode;
  /* Make sure we don't get into trouble again.  */
  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
};

typedef struct _IO_FILE _IO_FILE;

#define _IO_FLAGS2_FORTIFY 4
#define _IO_FLAGS2_SCANF_STD 16
#define _IO_ERR_SEEN 0x20

#define _IO_ferror_unlocked(__fp) (((__fp)->_flags & _IO_ERR_SEEN) != 0)

#define _IO_acquire_lock(_fp)                        \
  do {                                 \
    _IO_FILE *_IO_acquire_lock_file = NULL
#define _IO_acquire_lock_clear_flags2(_fp)                 \
  do {                                 \
    _IO_FILE *_IO_acquire_lock_file = (_fp)
#define _IO_release_lock(_fp)                        \
    if (_IO_acquire_lock_file != NULL)                   \
      _IO_acquire_lock_file->_flags2 &= ~(_IO_FLAGS2_FORTIFY            \
                                          | _IO_FLAGS2_SCANF_STD);         \
  } while (0)
