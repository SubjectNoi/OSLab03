//
// Created by 10152130243 and 10152130247 on 11/3/17.
//            OSLab03: Malloc And Free
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "mem.h"

#define MAGIC_NUM 36146655

typedef  struct __Header {
    int size;                        // Size of a region exclude the header.
    int magic;                       // Magic number
} Header;

int m_error;
void *start_add;                     // Start of the RAM pool initialized by mem_init(int)
void *end_add;                       // End of the RAM pool initialized by mem_init(int)

/*
 *  Round up 'in' to 'base', like round(1, 8) = 8, round(5, 4) = 8, round(17, 16) = 32
 *  args:   in      :input number
 *          base    :base number
 *  return: the round-up number
 */
int round(int in, int base) {
    int rest = in % base;
    if (!rest) return in;
    else {
        return in - rest + base;
    }
}

/*
 *  Merge RAM fragments.
 *  args:   *__start    :start of the RAM pool initialized by mem_init()
 *          *__end      :end of the RAM pool initialized by mem_init()
 */
void mem_merge(void *__start, void *__end) {
    void *cur = __start;
    while (cur != __end) {
        Header *cur_header = (Header*)cur;
        if (cur_header -> magic == MAGIC_NUM) {
            cur += ((cur_header -> size) + sizeof(Header));
        }
        else {
            Header *next_header = (Header*)(cur + cur_header -> size + sizeof(Header));
            if ((void*)next_header == end_add) break;
            if (next_header -> magic == MAGIC_NUM) {
                cur += ((cur_header -> size) + sizeof(Header));
            }
            else {
                cur_header -> size += ((next_header -> size) + sizeof(Header));
                cur += (((Header*)cur) -> size + sizeof(Header));
            }
        }
    }
}

/*
 *  Initialize a region of RAM pool, which all malloc, free operations are operated in this region.
 *  args:   size_of_region  :the size you want.
 *  return: return 0 if success, otherwise something goes wrong.
 *          *start_add record the start of this region
 *          *end_add record the end of this region
 */
int mem_init(int size_of_region) {
    if (size_of_region < 0) return E_BAD_ARGS;
    int page_size = getpagesize();
    size_of_region += sizeof(Header);
    size_of_region = round(size_of_region, page_size);
    int pool = open("/dev/zero", O_RDWR);
    start_add = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE, pool, 0);
    if (start_add == MAP_FAILED) {
        perror("mmap error.");
        return 1;
    }
    close(pool);
    Header *cur = (Header*)start_add;
    cur -> magic = 0;
    cur -> size = size_of_region - sizeof(Header);
    end_add = start_add + size_of_region;
    //mem_dump();
    return 0;
}

/*
 *  Allocate a region of RAM in RAM pool to the user.
 *  args:   size    :size that user want
 *          style   :algorithm of allocation
 *  return: return the address of available region, if NULL, there's no enough place to allocate.
 */
void* mem_alloc(int size, int style) {
    size += sizeof(Header);
    size = round(size, 8);
    void *cur_add = start_add;
    Header *cur_add_header;
    void *add_return = 0;
    switch (style) {
        case M_FIRSTFIT:
            while (cur_add != end_add) {
                cur_add_header = (Header*)cur_add;
                if (cur_add_header -> magic == 0 && cur_add_header -> size >= size) {
                    add_return = cur_add;
                    break;
                }
                cur_add += ((cur_add_header -> size) + sizeof(Header));
            }
            break;
        case M_BESTFIT:
            while (cur_add != end_add) {
                cur_add_header = (Header*)cur_add;
                if (cur_add_header -> magic == 0 && cur_add_header -> size >= size && (!add_return || cur_add_header -> size < ((Header*)add_return) -> size)) {
                    add_return = cur_add;
                }
                cur_add += ((cur_add_header -> size) + sizeof(Header));
            }
            break;
        case M_WORSTFIT:
            while (cur_add != end_add) {
                cur_add_header = (Header*)cur_add;
                if (cur_add_header -> magic == 0 && cur_add_header -> size >= size && (!add_return || cur_add_header -> size > ((Header*)add_return) -> size)) {
                    add_return = cur_add;
                }
                cur_add += ((cur_add_header -> size) + sizeof(Header));
            }
            break;

    }
    if (!add_return) {
        m_error = E_NO_SPACE;
        return add_return;
    }
    Header* add_return_header = (Header*)add_return;
    if (add_return_header -> size > size) {
        ((Header*)(add_return + size + sizeof(Header))) -> magic = 0;
        ((Header*)(add_return + size + sizeof(Header))) -> size = add_return_header -> size - size - sizeof(Header);
    }
    add_return_header -> magic = MAGIC_NUM;
    add_return_header -> size = size - sizeof(Header);
    add_return += sizeof(Header);
    mem_merge(start_add, end_add);
    return add_return;
}

/*
 *  Free a region user assigned
 *  args:   *ptr    :address of the region user want to free
 *  return: return 0 if success, -1 if origin pointer is NULL
 */
int mem_free(void* ptr) {
    if (!ptr) return -1;
    ptr -= sizeof(Header);
    Header *ptr_header = (Header*)ptr;
    ptr_header -> magic = 0;
    mem_merge(start_add, end_add);
    return 0;
}

/*
 *  Print the snap-shot of current RAM pool.
 */
void mem_dump() {
    printf("start_add = 0x%x.\n", (unsigned int)start_add);
    void *cur_add = start_add;
    while (cur_add != end_add) {
        printf("Space from 0x%x to 0x%x, magic = %8d, size = %d.\n", (unsigned int)(cur_add + sizeof(Header)), (unsigned int)(cur_add + sizeof(Header) + ((Header*)cur_add) -> size), ((Header*)cur_add) -> magic, ((Header*)cur_add) -> size);
        cur_add += (sizeof(Header) + ((Header*)cur_add) -> size);
    }
    printf("end_add = 0x%x.\n", (unsigned int)end_add);
    printf("-----------------------------------------------------------------------\n");
}
