#ifndef __SM3TST2_H__
#define __SM3TST2_H__

#include "common.h"

typedef enum
{
    SM32_ERROR_PARAM_NULL = 0x0001,
    SM32_ERROR_PARAM_OVERSHOOT,
    SM32_ERROR_CORE_SYMBOL_NULL,
    SM32_ERROR_MODULE_NOT_INITIALIZED = 0x00FF
} SM32_ERROR_ENUM;

void SM32_Init(void);

void SM32_Compress(uint8_t *msg, uint32_t len);

uint8_t * SM32_Hash(void);

#endif /* #ifndef __SM3TST2_H__ */
