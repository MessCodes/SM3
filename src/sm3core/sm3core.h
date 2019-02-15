#ifndef __SM3CORE_H__
#define __SM3CORE_H__

#include "common.h"

#define SM3CORE_SYMBOLS(s)  void *(s)

#if defined(WIN32)
# ifdef __cplusplus
  #define __CPP     extern "C"
# else
  #define __CPP
# endif

# ifdef __NATIVE_CODE
  #define DllImport __CPP
  #define DllExport __CPP
# else
  #define DllImport __CPP __declspec( dllimport )
  #define DllExport __CPP __declspec( dllexport )
# endif
#endif

#if defined(WIN32)
# ifdef SM3CORE_EXPORTS
  #define IMPORT    DllImport
  #define EXPORT    DllExport
# else
  #define IMPORT    DllImport
  #define EXPORT    DllImport
# endif
#elif defined(PK51)
  #define IMPORT    extern
  #define EXPORT
#endif

EXPORT void sm3core_start(void);

#endif /* #ifdef __SM3CORE_H__ */
