#ifndef _MD5_UTIL_H_
#define _MD5_UTIL_H_

#ifndef uint32
#  define uint32 unsigned int
#endif

struct Context {
  uint32 buf[4];
  uint32 bits[2];
  unsigned char in[64];
};
typedef char MD5Context[88];
void MD5Init(MD5Context *pCtx);
void MD5Update(MD5Context *pCtx, const unsigned char *buf, unsigned int len);
void MD5Final(unsigned char digest[16], MD5Context *pCtx);
void DigestToBase16(unsigned char *digest, char *zBuf);
void DigestToBase16L(unsigned char *digest, char *zBuf);


#endif
