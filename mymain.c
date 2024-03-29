#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
/*
 *  ==========================TEST==============================
 */
int main(int argc, int *argv[]) {
    mem_init(sizeof(int) * 8192);
    void *tmp  = mem_alloc(sizeof(int) * 256, M_BESTFIT );
    void *tmp1 = mem_alloc(sizeof(int) * 128, M_FIRSTFIT);
    void *tmp2 = mem_alloc(sizeof(int) * 384, M_WORSTFIT);
    void *tmp3 = mem_alloc(sizeof(int) * 64 , M_BESTFIT );
    void *tmp4 = mem_alloc(sizeof(int) * 768, M_FIRSTFIT);
    mem_dump();
    mem_free(tmp1);
    mem_free(tmp2);
    mem_dump();
    mem_free(tmp3);
    mem_dump();
    void *tmp5 = mem_alloc(sizeof(int) * 500, M_WORSTFIT);
    mem_dump();
    return 0;
}
