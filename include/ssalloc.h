#ifndef SSALLOC_H
#define SSALLOC_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sched.h>
#include <inttypes.h>
#include <string.h>

#if GC == 1			/* don't even allocate ssalloc if we have ssmem */
#  define SSALLOC_USE_MALLOC
#endif

#define SSALLOC_NUM_ALLOCATORS 2

#ifdef TEST_TIME
#define SSALLOC_SIZE_BIG  1024 * 1024 * 1024
// #define SSALLOC_SIZE_SMALL 320 * 1024  * 1024 // bst i2000000 u50
#define SSALLOC_SIZE_SMALL 2 * 1024  * 1024 // ll i2000 u50
// #define SSALLOC_SIZE_SMALL 200 * 1024  * 1024 // sl i20000 u50
#else
#define SSALLOC_SIZE_BIG  1024 * 1024 * 1024
#define SSALLOC_SIZE_SMALL 1024 * 1024  * 1024
#endif

#if defined(__sparc__)
#  define SSALLOC_SIZE (128LL * 1024 * 1024)
#elif defined(__tile__)
#  define SSALLOC_SIZE (100 * 1024 * 1024)
#elif defined(LAPTOP) | defined(IGORLAPTOPLINUX) | defined(OANALAPTOPLINUX)
#  define SSALLOC_SIZE (100 * 1024 * 1024)
#else
#  define SSALLOC_SIZE (1024 * 1024 * 1024)
#endif

void ssalloc_set(void* mem);
void ssalloc_init();
void ssalloc_offset(size_t size);
void* ssalloc_alloc(unsigned int allocator, size_t size);
void* ssalloc_aligned_alloc(unsigned int allocator, size_t alignment, size_t size);
void ssfree_alloc(unsigned int allocator, void* ptr);
void* ssalloc(size_t size);
void* ssalloc_aligned(size_t alignment, size_t size);

void ssfree(void* ptr);

#endif
