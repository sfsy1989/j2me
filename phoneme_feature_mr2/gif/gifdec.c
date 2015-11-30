#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gif_lib.h"
#ifdef FV_TEST_GIF
#define pcsl_mem_malloc malloc
#define pcsl_mem_free free
#else
#include <pcsl_memory.h>
#endif

struct fv_dec_userdata {
	unsigned char *ibuf;
	int size;
	int off;
	unsigned char *buf;
	int trans;
	unsigned char trans_color;
};

static int interlaced_offset[] = {
	0, 4, 2, 1 
};
static int interlaced_jumps[] = {
	8, 8, 4, 2
};

static int
fv_read_gif_data(struct GifFileType *ft, GifByteType *buf, int size)
{
	struct fv_dec_userdata *u = ft->UserData;
	int ret = (u->off + size >  u->size) ? (u->size - u->off) : size;

#if 0
	if (size <= 0)
		return size;
#endif
	if (ret > 0) {
		memcpy(buf, u->ibuf+u->off, ret);
		u->off += ret;
		return ret;		
	} else {
		return 0;
	}
}

static int
proc_ext_type(struct GifFileType *ft)
{
	struct fv_dec_userdata *u = ft->UserData;
	GifByteType *data;
	int code;

	if (DGifGetExtension(ft, &code, &data) == GIF_ERROR) {
		return -1;
	}
	/**quoted from MPlayer*/
	if (code == 0xf9) {
		if (data[0] == 4) {  /**check length*/
			u->trans       = data[1] & 1;
			u->trans_color = data[4];
		}
	}
	while (data != NULL) {
		if (DGifGetExtensionNext(ft, &data) == GIF_ERROR) {
			//print_gif_error(ft);
			return 0; // oops
		}
	}
	
	return 0;
}

static int
proc_img_type(struct GifFileType *ft)
{
	struct fv_dec_userdata *u = ft->UserData;
	unsigned char *p;
	int x;
	int y;
	int w;
	int h;
	int pitch;
	int i;
	int j;

	if (DGifGetImageDesc(ft) == GIF_ERROR) {
		return -1;
	}
	x     = ft->Image.Left;
	y     = ft->Image.Top;
	w     = ft->Image.Width;
	h     = ft->Image.Height;
	pitch = ft->SWidth;

	if (x + w > ft->SWidth ||
	    y + h > ft->SHeight) {
		return -1;
	}

	p = u->buf + y * pitch + x;
	if (ft->Image.Interlace) {
		for (i = 0; i <	4; i++) {
			for (j = interlaced_offset[i];
			     j < h; j += interlaced_jumps[i]) {
				if (DGifGetLine(ft, p+pitch*j, w) == 
				    GIF_ERROR) {
					return -1;
				}
			}
		}
	} else {
		for (i = 0; i < h; i++) {
			if (DGifGetLine(ft, p+pitch*i, w) == GIF_ERROR) {
				return -1;
			}
		}
	}

	return 0;
}

		
static int 
fill_pixel_buf(struct GifFileType *ft,
               unsigned char *pixel,
               unsigned char *alpha)
{
	struct fv_dec_userdata *u = ft->UserData;
	unsigned char *s;
	unsigned char *d;
	ColorMapObject *cmap; 
	GifColorType *color;
	int w = ft->SWidth;
	int h = ft->SHeight;
		
	cmap = ft->Image.ColorMap ? ft->Image.ColorMap : ft->SColorMap;
	s = u->buf;
	d = pixel;

	if (u->trans) {
		unsigned char *a;
		a = alpha;
		for (; s < u->buf + w * h; s++, d+=3, a++) {
			if (u->trans_color != *s) {
				color = &cmap->Colors[*s];
				*(d + 0) = color->Blue;
				*(d + 1) = color->Green;
				*(d + 2) = color->Red;
				*a       = 0xff; 
			} else {
				*a       = 0;	
			} 
		}
	} else {
		for (; s < u->buf + w * h; s++, d+=3) {
			color = &cmap->Colors[*s];
			*(d + 0) = color->Blue;
			*(d + 1) = color->Green;
			*(d + 2) = color->Red;
		}
	}
	return 0;
}

/**return
 0: ok
-1: failed*/
int
fv_gif2rgb(unsigned char  *buf,
           int             size,
           unsigned char *pixel, 
           unsigned char *alpha,
           int            *has_alpha)
{
	struct GifFileType *ft = 0;
	GifRecordType type;
	int ret = 0;
	struct fv_dec_userdata *u;

	u = pcsl_mem_malloc(sizeof(struct fv_dec_userdata));
	if (!u) {
		return -1;
	}

	u->ibuf  = buf;
	u->size  = size;
	u->off   = 0;
	u->buf   = 0;
	u->trans = 0;
	ft = DGifOpen(u, fv_read_gif_data);
	if (!ft) {
		pcsl_mem_free(u);
		return -1;
	}
#if 0
	if (ft->ImageCount < 1) {
		DGifCloseFile(ft);
		pcsl_mem_free(u);
		return -1;
	}
#endif
	if (ft->SWidth <= 0 || ft->SHeight <= 0) {
		DGifCloseFile(ft);
		pcsl_mem_free(u);
		return -1;
	}

	u->buf = pcsl_mem_malloc(ft->SWidth*ft->SHeight);
	if (!u->buf) {
		DGifCloseFile(ft);
		pcsl_mem_free(u);
		return -1;
	}
	memset(u->buf, ft->SBackGroundColor, ft->SWidth*ft->SHeight);
	
	do {
		if (DGifGetRecordType(ft, &type) == GIF_ERROR) {
			ret = -1;
			break;
		}
		if (type == EXTENSION_RECORD_TYPE) {
			if (proc_ext_type(ft) < 0) {
				ret = -1;
				break;
			}
		} else if (type == IMAGE_DESC_RECORD_TYPE) {
			ret = proc_img_type(ft);	
			break;
		}
	} while(type != TERMINATE_RECORD_TYPE);

	if (!ret) {
		*has_alpha = u->trans;
		ret = fill_pixel_buf(ft, pixel, alpha);
	}
	
	DGifCloseFile(ft);
	pcsl_mem_free(u->buf);
	pcsl_mem_free(u);	
	return ret;
}

#ifdef FV_TEST_GIF
static void 
proc_gif(unsigned char *mem, int sz, int width, int height)
{
	int r;
	unsigned char *pixel;
	unsigned char *alpha;
	int has_alpha = 0;

	pixel = (unsigned char *)malloc(width * height * 3);
	if (!pixel) {
		return;
	}
	alpha = (unsigned char *)malloc(width * height);
	if (!alpha) {
		free(pixel);
		return;
	}
	r = fv_gif2rgb(mem, sz, pixel, alpha, &has_alpha);
	if (!r) {
		printf("gif proc OK : alpha: %d\n",
		       has_alpha);
	} else {
		printf("gif proc failed\n");
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	long sz;
	unsigned char *mem;
	int width;
	int height;

	if (argc != 4) {
		printf("Usage: %s giffile width height\n", argv[0]);
		return -1;
	}
	width  = atoi(argv[2]);
	height = atoi(argv[3]);
	if (width <= 0 || height <= 0) {
		printf("width height error\n");
		return -1;
	}
	
	fp = fopen(argv[1], "rb");
	if (!fp) {
		perror("fopen:");
		return -1;
	}
	
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	mem = malloc(sz);
	if (!mem) {
		printf("malloc failed\n");	
		fclose(fp);
		return -1;
	}
	
	if (sz != fread(mem, 1, sz, fp)) {
		perror("fread:");
		free(mem);
		fclose(fp);
	}
	
	proc_gif(mem, sz, width, height);
	
	free(mem);
	fclose(fp);
	return 0;
}
#endif

