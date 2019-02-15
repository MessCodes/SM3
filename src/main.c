#include <stdio.h>
#include <string.h>

#include "sm3tst0.h"
#include "sm3tst1.h"
#include "sm3tst2.h"

#if defined(CONFIG_TARGETS_TEST0) && (CONFIG_TARGETS_TEST0 == 1)

void SM30_Test(void)
{
    uint8_t *p;
    uint8_t i;

    SM30_Init();

#define MSG1 "abc"
    printf("%s:%d SM3 Start.\n", __FILE__, __LINE__);
    SM30_Compress(MSG1, strlen(MSG1));
    p = SM30_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");
#if 1
#define MSG2 "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd"
    SM30_Compress(MSG2, strlen(MSG2));
    p = SM30_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");

#define MSG3 "#include <stdio.h> int main(int argc, char *argv[]) " \
             "{ printf(\"Hello world!\"); return 0; }"
    SM30_Compress(MSG3, strlen(MSG3));
    p = SM30_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");
#endif
}

#endif

#if defined(CONFIG_TARGETS_TEST1) && (CONFIG_TARGETS_TEST1 == 1)

void SM31_Test(void)
{
    uint8_t *p;
    uint8_t i;

    SM31_Init();

#define MSG1 "abc"
    printf("%s:%d SM3 Start.\n", __FILE__, __LINE__);
    SM31_Compress(MSG1, strlen(MSG1));
    p = SM31_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");
#if 1
#define MSG2 "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd"
    SM31_Compress(MSG2, strlen(MSG2));
    p = SM31_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");

#define MSG3 "#include <stdio.h> int main(int argc, char *argv[]) " \
             "{ printf(\"Hello world!\"); return 0; }"
    SM31_Compress(MSG3, strlen(MSG3));
    p = SM31_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");
#endif
}

#endif

#if defined(CONFIG_TARGETS_TEST2) && (CONFIG_TARGETS_TEST2 == 1)

void SM32_Test(void)
{
    uint8_t *p;
    uint8_t i;

    SM32_Init();

#define MSG1 "abc"
    printf("%s:%d SM3 Start.\n", __FILE__, __LINE__);
    SM32_Compress(MSG1, strlen(MSG1));
    p = SM32_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");
#if 1
#define MSG2 "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd"
    SM32_Compress(MSG2, strlen(MSG2));
    p = SM32_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");

#define MSG3 "#include <stdio.h> int main(int argc, char *argv[]) " \
             "{ printf(\"Hello world!\"); return 0; }"
    SM32_Compress(MSG3, strlen(MSG3));
    p = SM32_Hash();
    for (i = 0; i < 32; i++)
    {
        printf("%02x", p[i]);
    }
    printf("\n");
#endif
}

#endif

int main(int argc, char *argv[])
{
#if defined(CONFIG_TARGETS_TEST0) && (CONFIG_TARGETS_TEST0 == 1)
    SM30_Test();
#endif
#if defined(CONFIG_TARGETS_TEST1) && (CONFIG_TARGETS_TEST1 == 1)
    SM31_Test();
#endif
#if defined(CONFIG_TARGETS_TEST2) && (CONFIG_TARGETS_TEST2 == 1)
    SM32_Test();
#endif

    getchar();

    return 0;
}
