#ifndef __SM3TST1_H__
#define __SM3TST1_H__

typedef enum
{
    SM31_ERROR_PARAM_NULL = 0x0001,
    SM31_ERROR_PARAM_OVERSHOOT,
    SM31_ERROR_CORE_SYMBOL_NULL,
    SM31_ERROR_MODULE_NOT_INITIALIZED = 0x00FF
} SM31_ERROR_ENUM;

void SM31_Init(void);

void SM31_Compress(uint8_t *msg, uint32_t len);

uint8_t * SM31_Hash(void);

#endif /* #ifndef __SM3TST1_H__ */
