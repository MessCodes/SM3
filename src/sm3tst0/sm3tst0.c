#include <stdio.h>
#include <string.h>

#include "sm3core.h"

#include "sm3tst0.h"

//////
#if defined(WIN32)
#pragma warning(disable: 4090)
#endif
//////

IMPORT SM3CORE_SYMBOLS(__syms_sm3_base);

#define SM30_start()    do { sm3core_start(); } while(0)
#define SM30_status()   (1)
#define SM30_stop()     do { } while (!SM30_status())

#pragma pack(1)

typedef struct sm30_ramap
{
    uint8_t msg[64];
    uint8_t median[32];
    union {
        uint32_t    word[8];
        uint8_t     byte[32];
    } hash;
} SM30_RAMAP_STRUCT;

LOCAL struct {
    volatile uint16_t   errno;
    struct {
        volatile uint32_t   msb, lsb; /* Bits, Stored in big endian */
    } msglen;
    union {
        volatile uint32_t   word[8];
        volatile uint8_t    byte[32];
    } hash;
} sm30_ctx_res = {
    /* .errno = */ SM30_ERROR_MODULE_NOT_INITIALIZED,
    /* .msglen = */ {HTONL(0u), HTONL(0u)}
};

#pragma pack()

LOCAL volatile SM30_RAMAP_STRUCT *sm30_ram = NULL;

void SM30_Init(void)
{
    if (__syms_sm3_base)
    {
        sm30_ram = (SM30_RAMAP_STRUCT *)__syms_sm3_base;

        memset((uint8_t *)sm30_ram, 0, sizeof(SM30_RAMAP_STRUCT));
        sm30_ram->hash.word[0] = HTONL(0x7380166f);
        sm30_ram->hash.word[1] = HTONL(0x4914b2b9);
        sm30_ram->hash.word[2] = HTONL(0x172442d7);
        sm30_ram->hash.word[3] = HTONL(0xda8a0600);
        sm30_ram->hash.word[4] = HTONL(0xa96f30bc);
        sm30_ram->hash.word[5] = HTONL(0x163138aa);
        sm30_ram->hash.word[6] = HTONL(0xe38dee4d);
        sm30_ram->hash.word[7] = HTONL(0xb0fb0e4e);

        sm30_ctx_res.msglen.msb = HTONL(0u);
        sm30_ctx_res.msglen.lsb = HTONL(0u);
        memset((uint8_t *)&(sm30_ctx_res.hash), 0, sizeof(sm30_ctx_res.hash));

        sm30_ctx_res.errno = OK;
    }
    else
    {
        sm30_ctx_res.errno = SM30_ERROR_CORE_SYMBOL_NULL;
    }
}

void SM30_Compress(uint8_t *msg, uint32_t len)
{
    uint8_t ofs = (NTOHL(sm30_ctx_res.msglen.lsb)>>3)&0x3f;

    if (sm30_ctx_res.errno == OK)
    {
        if (CHK_PTR(msg))
        {
            sm30_ctx_res.errno = SM30_ERROR_PARAM_NULL;
        }

        if (sm30_ctx_res.errno == OK)
        {
            sm30_ctx_res.msglen.lsb = HTONL( \
                        ((NTOHL(sm30_ctx_res.msglen.lsb)>>3)+len)<<3 \
                    );
            if ((NTOHL(sm30_ctx_res.msglen.lsb)>>3) < len)
            {
                sm30_ctx_res.msglen.msb = HTONL( \
                            ((NTOHL(sm30_ctx_res.msglen.msb)>>3)+1)<<3 \
                        );
            }

#define SM30_calc()     do { SM30_start(); SM30_stop(); } while(0)
            if (len >= (uint8_t)(64-ofs))
            {
                memcpy(sm30_ram->msg+ofs, msg, 64-ofs);
                
                SM30_calc();

                for (msg+=(64-ofs), len-=(64-ofs); len > 63; msg+=64, len-=64)
                {
                    memcpy(sm30_ram->msg, msg, 64);

                    SM30_calc();
                }
                ofs = 0;
            }

            if (len > 0)
            {
                memcpy(sm30_ram->msg+ofs, msg, (uint32_t)len);
            }
        }
    }
    else
    {
        sm30_ctx_res.errno = SM30_ERROR_MODULE_NOT_INITIALIZED;
    }
}

uint8_t * SM30_Hash(void)
{
    uint32_t mbits[2];
    uint8_t ofs = (NTOHL(sm30_ctx_res.msglen.lsb)>>3)&0x3f;
    
    LOCAL uint8_t TEXT sm30_fill[64] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    memset((uint8_t *)&(sm30_ctx_res.hash), 0, sizeof(sm30_ctx_res.hash));

    mbits[0] = sm30_ctx_res.msglen.msb;
    mbits[1] = sm30_ctx_res.msglen.lsb;

    if (sm30_ctx_res.errno == OK)
    {
        SM30_Compress(sm30_fill, ((119-ofs)&0x3f)+1);
        SM30_Compress((uint8_t *)mbits, sizeof(mbits));

        memcpy(sm30_ctx_res.hash.byte, sm30_ram->hash.byte, 
                sizeof(sm30_ctx_res.hash));

        sm30_ram->hash.word[0] = HTONL(0x7380166f);
        sm30_ram->hash.word[1] = HTONL(0x4914b2b9);
        sm30_ram->hash.word[2] = HTONL(0x172442d7);
        sm30_ram->hash.word[3] = HTONL(0xda8a0600);
        sm30_ram->hash.word[4] = HTONL(0xa96f30bc);
        sm30_ram->hash.word[5] = HTONL(0x163138aa);
        sm30_ram->hash.word[6] = HTONL(0xe38dee4d);
        sm30_ram->hash.word[7] = HTONL(0xb0fb0e4e);

        sm30_ctx_res.msglen.msb = HTONL(0u);
        sm30_ctx_res.msglen.lsb = HTONL(0u);
    }

    return sm30_ctx_res.hash.byte;
}
