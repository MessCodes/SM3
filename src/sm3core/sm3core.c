#include <stdio.h>
#include <string.h>

#include "sm3core.h"

// SECTIONS DEFINES.
#pragma section(".sm3.reg", read, write)
#pragma section(".sm3.ram", read, write)
#pragma section(".sm3.rom", read)

// DATA TYPE DEFINES.
#pragma pack(1)

typedef struct sm3core_buff
{
	__IO uint8_t    msg[64];
	__IO uint32_t   A, B, C, D, E, F, G, H;
	__IO uint32_t   IV[8];
} sm3core_buff_t;

#pragma pack()

// MICROS DEFINES.
#define ROL(e, n)       HTONL((NTOHL(e)<<((n)%32))|(NTOHL(e)>>(32-((n)%32))))
#define ROR(e, n)       HTONL((NTOHL(e)>>((n)%32))|(NTOHL(e)<<(32-((n)%32))))
#define BAND(x, y)      HTONL(NTOHL(x)&NTOHL(y))
#define BOR(x, y)       HTONL(NTOHL(x)|NTOHL(y))
#define BOR3(x, y, z)   HTONL(NTOHL(x)|NTOHL(y)|NTOHL(z))
#define BNOT(x)         HTONL(~(NTOHL(x)))
#define XOR(x, y)       HTONL(NTOHL(x)^NTOHL(y))
#define XOR3(x, y, z)   HTONL(NTOHL(x)^NTOHL(y)^NTOHL(z))

/**
 *             |- X^Y^Z                0<=j<=15
 * FFj(X,Y,Z)=<
 *             |- (X&Y)|(X&Z)|(Y&Z)    16<=j<=63
 *
 *             |- X^Y^Z                0<=j<=15
 * GGj(X,Y,Z)=<
 *             |- (X&Y)|(~X&Z)         16<=j<=63
 * ** NOTE: The X/Y/Z defaults to big endian. **
 */
#define FFj(j, X, Y, Z) (((j)&0xF0)?BOR3(BAND(X,Y),BAND(X,Z),BAND(Y,Z)):XOR3(X,Y,Z))
#define GGj(j, X, Y, Z) (((j)&0xF0)?BOR(BAND(X,Y),BAND(BNOT(X),Z)):XOR3(X,Y,Z))
/**
 * P0(X)=X^(X<<<9)^(X<<<17)
 * P1(X)=X^(X<<<15)^(X<<<23)
 * ** NOTE: The X defaults to big endian. **
 */
#define P0(X)   HTONL(NTOHL(X)^NTOHL(ROL((X), 9))^NTOHL(ROL((X), 17)))
#define P1(X)   HTONL(NTOHL(X)^NTOHL(ROL((X), 15))^NTOHL(ROL((X), 23)))

// SM3 core regfile.
LOCAL __declspec(allocate(".sm3.reg")) __IO uint32_t W[68];
LOCAL __declspec(allocate(".sm3.reg")) __IO uint32_t Wp[64];
LOCAL __declspec(allocate(".sm3.reg")) __IO uint32_t SS1, SS2, TT1, TT2;

// SM3 core ram buffer.
LOCAL __declspec(allocate(".sm3.ram")) struct sm3core_buff sm3core_ram;

// SM3 core rom data.
LOCAL __declspec(allocate(".sm3.rom")) TEXT uint32_t Tj[2] = {HTONL(0x79cc4519), HTONL(0x7a879d8a)};

// SM3 core export symbols.
EXPORT __declspec(allocate(".sm3.rom")) TEXT void *__syms_sm3_base = &sm3core_ram;

// SM3 core algorithm execution unit.
#pragma code_seg(".sm3.nor")

EXPORT void sm3core_start(void)
{
    uint8_t i;

    // TODO: Message extension.
    /* a). The message packet B(i) is divided into 16 words W0, W1, ..., W15. */
    for (i = 0; i < 64; i++)
    {
        ((uint8_t *)W)[i] = sm3core_ram.msg[i];
    }

    /**
     * b). Further expansion of W16, W17, ..., W67.
     *   FOR j=16 TO 67
     *     W[j] <- P1(W[j-16] ^ W[j-9] ^ (W[j-3]<<<15)) ^ (W[j-13]<<<7) ^ W[j-6]
     *   ENDFOR
     */
    for (i = 16; i < 68; i++)
    {
#define P1_ARG0(j)  XOR3(W[j-16],W[j-9], ROL(W[j-3],15))
#if 0
        W[i] = XOR3(P1(P1_ARG0(i)), ROL(W[i-13], 7), W[i-6]);
#else
        W[i] = P1_ARG0(i);
        W[i] = P1(W[i]);
        W[i] = XOR3(W[i], ROL(W[i-13], 7), W[i-6]);
#endif
    }

    /**
     * c). Extend Wp0, Wp1, ..., Wp63 according to W0, W2, ..., W67.
     *   FOR j=0 TO 63
     *     Wp[j] = W[j] ^ W[j+4]
     *   ENDFOR
     */
    for (i = 0; i < 64; i++)
    {
        Wp[i] = XOR(W[i], W[i+4]);
    }

    // TODO: Compression
    /* 1. ABCDEFGH <- V(i) */
    sm3core_ram.A = sm3core_ram.IV[0];
    sm3core_ram.B = sm3core_ram.IV[1];
    sm3core_ram.C = sm3core_ram.IV[2];
    sm3core_ram.D = sm3core_ram.IV[3];
    sm3core_ram.E = sm3core_ram.IV[4];
    sm3core_ram.F = sm3core_ram.IV[5];
    sm3core_ram.G = sm3core_ram.IV[6];
    sm3core_ram.H = sm3core_ram.IV[7];

    /**
     * 2. FOR j=0 TO 63
     *      SS1 <- ((A<<<12) + E + (Tj<<<j)) <<< 7
     *      SS2 <- SS1 ^ (A<<<12)
     *      TT1 <- FFj(A,B,C) + D + SS2 + Wp[j]
     *      TT2 <- GGj(E,F,G) + H + SS1 + W[j]
     *      D <- C
     *      C <- B<<<9
     *      B <- A
     *      A <- TT1
     *      H <- G
     *      G <- F<<<19
     *      F <- E
     *      E <- P0(TT2)
     *    ENDFOR
     */
    for (i = 0; i < 64; i++)
    {
        /* 2.1. SS1 <- ((A<<<12) + E + (Tj<<<j)) <<< 7 */
        SS1 = ROL(HTONL( \
                NTOHL(ROL(sm3core_ram.A,12)) + \
                    NTOHL(sm3core_ram.E) + \
                        NTOHL(ROL(Tj[(i&0xF0)?1:0],i))), 7);

        /* 2.2. SS2 <- SS1 ^ (A<<<12) */
        SS2 = XOR(SS1, ROL(sm3core_ram.A, 12));

        /* 2.3. TT1 <- FFj(A,B,C) + D + SS2 + Wp[j] */
        TT1 = HTONL( \
                NTOHL(FFj(i,sm3core_ram.A,sm3core_ram.B,sm3core_ram.C)) + \
                    NTOHL(sm3core_ram.D) + \
                        NTOHL(SS2) + NTOHL(Wp[i]));

        /* 2.4. TT2 <- GGj(E,F,G) + H + SS1 + W[j] */
        TT2 = HTONL( \
                NTOHL(GGj(i, sm3core_ram.E,sm3core_ram.F,sm3core_ram.G)) + \
                    NTOHL(sm3core_ram.H) + \
                        NTOHL(SS1) + NTOHL(W[i]));

        /* 2.5. D <- C */
        sm3core_ram.D = sm3core_ram.C;
        /* 2.6. C <- B<<<9 */
        sm3core_ram.C = ROL(sm3core_ram.B, 9);
        /* 2.7. B <- A */
        sm3core_ram.B = sm3core_ram.A;
        /* 2.8. A <- TT1 */
        sm3core_ram.A = TT1;
        /* 2.9. H <- G */
        sm3core_ram.H = sm3core_ram.G;
        /* 2.A. G <- F<<<19 */
        sm3core_ram.G = ROL(sm3core_ram.F, 19);
        /* 2.B. F <- E */
        sm3core_ram.F = sm3core_ram.E;
        /* 2.C. E <- P0(TT2) */
        sm3core_ram.E = P0(TT2);
    }

    /* 3. V(i+1) <- ABCDEFGH ^ V(i) */
    sm3core_ram.IV[0] = XOR(sm3core_ram.A, sm3core_ram.IV[0]);
    sm3core_ram.IV[1] = XOR(sm3core_ram.B, sm3core_ram.IV[1]);
    sm3core_ram.IV[2] = XOR(sm3core_ram.C, sm3core_ram.IV[2]);
    sm3core_ram.IV[3] = XOR(sm3core_ram.D, sm3core_ram.IV[3]);
    sm3core_ram.IV[4] = XOR(sm3core_ram.E, sm3core_ram.IV[4]);
    sm3core_ram.IV[5] = XOR(sm3core_ram.F, sm3core_ram.IV[5]);
    sm3core_ram.IV[6] = XOR(sm3core_ram.G, sm3core_ram.IV[6]);
    sm3core_ram.IV[7] = XOR(sm3core_ram.H, sm3core_ram.IV[7]);
}

#pragma code_seg()