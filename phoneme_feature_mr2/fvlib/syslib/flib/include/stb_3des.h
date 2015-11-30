#ifndef _DES_H
#define _DES_H

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
    uint32 esk[32];     /* DES encryption subkeys */
    uint32 dsk[32];     /* DES decryption subkeys */
}
des_context;

typedef struct
{
    uint32 esk[96];     /* Triple-DES encryption subkeys */
    uint32 dsk[96];     /* Triple-DES decryption subkeys */
}
des3_context;

//int  des_set_key( des_context *ctx, uint8 *key);
//void des_encrypt( des_context *ctx, uint8 *input, uint8 *output,int len);
//void des_decrypt( des_context *ctx, uint8 *input, uint8 *output,int len);
/**
 * \brief          DES-CBC buffer encryption
 *
 * \param ctx      DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be encrypted
 */
void des_cbc_encrypt( des_context *ctx,
                      uint8 iv[8],
                      uint8 *input,
                      uint8 *output,
                      int len );
/**
 * \brief          DES-CBC buffer encryption
 *
 * \param ctx      DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be decrypted
 */
void des_cbc_decrypt( des_context *ctx,
                      uint8 iv[8],
                      uint8 *input,
                      uint8 *output,
                      int len );



int  des3_set_2keys( des3_context *ctx, uint8 *key);
int  des3_set_3keys( des3_context *ctx, uint8 *key);

void des3_encrypt( des3_context *ctx, uint8 *input, uint8 *output,int len);
void des3_decrypt( des3_context *ctx, uint8 *input, uint8 *output,int len);

/**
 * \brief          3DES-CBC buffer encryption
 *
 * \param ctx      3DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be encrypted
 */
void des3_cbc_encrypt( des3_context *ctx,
                       uint8 iv[8],
                       uint8 *input,
                       uint8 *output,
                       int len );


/**
 * \brief          3DES-CBC buffer decryption
 *
 * \param ctx      3DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the ciphertext
 * \param output   buffer holding the plaintext
 * \param len      length of the data to be decrypted
 */
void des3_cbc_decrypt( des3_context *ctx,
                       uint8 iv[8],
                       uint8 *input,
                       uint8 *output,
                       int len );

int zg_3des_encrypt(char okey[24],unsigned char* in_buff, unsigned char* out_buff);
int h_3des_encrypt(char okey[24],  unsigned char* in_buff,unsigned char* out_buff);
int iptv2x_3des_encrypt(char okey[24],unsigned char* in_buff,unsigned char* out_buff);
int iptv2x_3des_decrypt(char okey[24], unsigned char *in_buff,unsigned char *out_buff);
#endif /* des.h */

