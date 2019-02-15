#ifndef __COMMON_H__
#define __COMMON_H__

typedef signed char         sint8_t;
typedef unsigned char       uint8_t;
typedef signed short        sint16_t;
typedef unsigned short      uint16_t;
typedef signed long         sint32_t;
typedef unsigned long       uint32_t;
typedef signed long long    sint64_t;
typedef unsigned long long  uint64_t;

#ifndef OK
  #define OK                (0)
#endif

#ifndef NULL
  #define NULL              ((void *)0)
#endif

#ifndef LOCAL
  #define LOCAL             static
#endif

#ifndef NELEMENTS
  #define NELEMENTS(e)      (sizeof(e) / sizeof((e)[0]))
#endif

#ifndef CHK_PTR
  #define CHK_PTR(p)        ((p) == NULL)
#endif

#if defined(WIN32) /* Microsoft Visual Studio IDE */
  #define TEXT              const

  #define __IO              volatile

  #define INLINE            inline
  #define LOCAL_INLINE      static inline
#elif defined(PK51) /* C51 Compiler */
  #define TEXT              code

  #define INLINE
  #define LOCAL_INLINE
#elif defined(DK251) /* C251 Compiler */
  #define TEXT              code

  #define INLINE
  #define LOCAL_INLINE
#elif defined(__CC_ARM) /* RealView Compiler */
  #define TEXT              const

  #define INLINE            __inline
  #define LOCAL_INLINE      static __inline
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
  #define TEXT              const

  #define INLINE            __inline
  #define LOCAL_INLINE      static __inline
#elif defined(__GNUC__) /* GNU Compiler */
  #define TEXT              const

  #define INLINE            inline
  #define LOCAL_INLINE      static inline
#elif defined(__ICCARM__) /* ICC Compiler */
  #define TEXT              const

  #define INLINE            inline
  #define LOCAL_INLINE      static inline
#else
  #error "Unknown Compiler."
#endif

#define HTONS(x)            (x)
#define HTONL(x)            (x)
#define HTONLL(x)           (x)
#define NTOHS(x)            (x)
#define NTOHL(x)            (x)
#define NTOHLL(x)           (x)

#ifdef __LITTLE_ENDIAN
  #undef HTONS
# define HTONS(x)           ((((x)&0xFF)<<8)|(((x)&0xFF00)>>8))
  #undef HTONL
# define HTONL(x)           ((((x)&0xFF)<<24)|(((x)&0xFF00)<<8)| \
                            (((x)&0xFF0000)>>8)|(((x)&0xFF000000)>>24))
  #undef HTONLL
# define HTONLL(x)          ((((x)&0xFF)<<56)|(((x)&0xFF00)<<40)| \
                            (((x)&0xFF0000)<<24)|(((x)&0xFF000000)<<8)| \
                            (((x)&0xFF00000000)>>8)| \
                            (((x)&0xFF0000000000)>>24)| \
                            (((x)&0xFF000000000000)>>40)| \
                            (((x)&0xFF00000000000000)>>56))
  #undef NTOHS
# define NTOHS(x)           ((((x)&0xFF)<<8)|(((x)&0xFF00)>>8))
  #undef NTOHL
# define NTOHL(x)           ((((x)&0xFF)<<24)|(((x)&0xFF00)<<8)| \
                            (((x)&0xFF0000)>>8)|(((x)&0xFF000000)>>24))
  #undef NTOHLL
#endif

#endif /* #ifndef __COMMON_H__ */
