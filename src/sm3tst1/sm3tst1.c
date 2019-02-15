#include <stdio.h>
#include <string.h>

#include "sm3core.h"

#include "sm3tst1.h"

//////
#if defined(WIN32)
#pragma warning(disable: 4090)
#endif
//////

#pragma comment(lib, "sm3core.lib")

IMPORT SM3CORE_SYMBOLS(__syms_sm3_base);

#define SM31_start()    do { sm3core_start(); } while(0)
#define SM31_status()   (1)
#define SM31_stop()     do { } while (!SM31_status())

#pragma pack(1)

typedef struct sm31_ramap
{
    uint8_t msg[64];
    uint8_t median[32];
    union {
        uint32_t    word[8];
        uint8_t     byte[32];
    } hash;
} SM31_RAMAP_STRUCT;

LOCAL struct {
    volatile uint16_t   errno;
    struct {
        volatile uint32_t   msb, lsb; /* Bits, Stored in big endian */
    } msglen;
    union {
        volatile uint32_t   word[8];
        volatile uint8_t    byte[32];
    } hash;
} sm31_ctx_res = {
    /* .errno = */ SM31_ERROR_MODULE_NOT_INITIALIZED,
    /* .msglen = */ {HTONL(0u), HTONL(0u)}
};

#pragma pack()

LOCAL volatile SM31_RAMAP_STRUCT *sm31_ram = NULL;

void SM31_Init(void)
{
    if (__syms_sm3_base)
    {
        sm31_ram = (SM31_RAMAP_STRUCT *)__syms_sm3_base;

        memset((uint8_t *)sm31_ram, 0, sizeof(SM31_RAMAP_STRUCT));
        sm31_ram->hash.word[0] = HTONL(0x7380166f);
        sm31_ram->hash.word[1] = HTONL(0x4914b2b9);
        sm31_ram->hash.word[2] = HTONL(0x172442d7);
        sm31_ram->hash.word[3] = HTONL(0xda8a0600);
        sm31_ram->hash.word[4] = HTONL(0xa96f30bc);
        sm31_ram->hash.word[5] = HTONL(0x163138aa);
        sm31_ram->hash.word[6] = HTONL(0xe38dee4d);
        sm31_ram->hash.word[7] = HTONL(0xb0fb0e4e);

        sm31_ctx_res.msglen.msb = HTONL(0u);
        sm31_ctx_res.msglen.lsb = HTONL(0u);
        memset((uint8_t *)&(sm31_ctx_res.hash), 0, sizeof(sm31_ctx_res.hash));

        sm31_ctx_res.errno = OK;
    }
    else
    {
        sm31_ctx_res.errno = SM31_ERROR_CORE_SYMBOL_NULL;
    }
}

void SM31_Compress(uint8_t *msg, uint32_t len)
{
    uint8_t ofs = (NTOHL(sm31_ctx_res.msglen.lsb)>>3)&0x3f;

    if (sm31_ctx_res.errno == OK)
    {
        if (CHK_PTR(msg))
        {
            sm31_ctx_res.errno = SM31_ERROR_PARAM_NULL;
        }

        if (sm31_ctx_res.errno == OK)
        {
            sm31_ctx_res.msglen.lsb = HTONL( \
                        ((NTOHL(sm31_ctx_res.msglen.lsb)>>3)+len)<<3 \
                    );
            if ((NTOHL(sm31_ctx_res.msglen.lsb)>>3) < len)
            {
                sm31_ctx_res.msglen.msb = HTONL( \
                            ((NTOHL(sm31_ctx_res.msglen.msb)>>3)+1)<<3 \
                        );
            }

#define SM31_calc()     do { SM31_start(); SM31_stop(); } while(0)
            if (len >= (uint8_t)(64-ofs))
            {
                memcpy(sm31_ram->msg+ofs, msg, 64-ofs);
                
                SM31_calc();

                for (msg+=(64-ofs), len-=(64-ofs); len > 63; msg+=64, len-=64)
                {
                    memcpy(sm31_ram->msg, msg, 64);

                    SM31_calc();
                }
                ofs = 0;
            }

            if (len > 0)
            {
                memcpy(sm31_ram->msg+ofs, msg, (uint32_t)len);
            }
        }
    }
    else
    {
        sm31_ctx_res.errno = SM31_ERROR_MODULE_NOT_INITIALIZED;
    }
}

uint8_t * SM31_Hash(void)
{
    uint32_t mbits[2];
    uint8_t ofs = (NTOHL(sm31_ctx_res.msglen.lsb)>>3)&0x3f;
    
    LOCAL uint8_t TEXT sm31_fill[64] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    memset((uint8_t *)&(sm31_ctx_res.hash), 0, sizeof(sm31_ctx_res.hash));

    mbits[0] = sm31_ctx_res.msglen.msb;
    mbits[1] = sm31_ctx_res.msglen.lsb;

    if (sm31_ctx_res.errno == OK)
    {
        SM31_Compress(sm31_fill, ((119-ofs)&0x3f)+1);
        SM31_Compress((uint8_t *)mbits, sizeof(mbits));

        memcpy(sm31_ctx_res.hash.byte, sm31_ram->hash.byte, 
                sizeof(sm31_ctx_res.hash));

        sm31_ram->hash.word[0] = HTONL(0x7380166f);
        sm31_ram->hash.word[1] = HTONL(0x4914b2b9);
        sm31_ram->hash.word[2] = HTONL(0x172442d7);
        sm31_ram->hash.word[3] = HTONL(0xda8a0600);
        sm31_ram->hash.word[4] = HTONL(0xa96f30bc);
        sm31_ram->hash.word[5] = HTONL(0x163138aa);
        sm31_ram->hash.word[6] = HTONL(0xe38dee4d);
        sm31_ram->hash.word[7] = HTONL(0xb0fb0e4e);

        sm31_ctx_res.msglen.msb = HTONL(0u);
        sm31_ctx_res.msglen.lsb = HTONL(0u);
    }

    return sm31_ctx_res.hash.byte;
}
