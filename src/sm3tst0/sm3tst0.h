#ifndef __SM3TST0_H__
#define __SM3TST0_H__

#include "common.h"

typedef enum
{
    SM30_ERROR_PARAM_NULL = 0x0001,
    SM30_ERROR_PARAM_OVERSHOOT,
    SM30_ERROR_CORE_SYMBOL_NULL,
    SM30_ERROR_MODULE_NOT_INITIALIZED = 0x00FF
} SM30_ERROR_ENUM;

void SM30_Init(void);

void SM30_Compress(uint8_t *msg, uint32_t len);

uint8_t * SM30_Hash(void);

#endif /* #ifndef __SM3TST0_H__ */
