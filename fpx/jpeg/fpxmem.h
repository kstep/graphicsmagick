/*

  Temporary implementation of a memory manager in the 
  JPEG library used in FlashPix.


  We have define the operators FPXMalloc and FPXFree


  */



#ifndef FPX_MEMORY_H
#define FPX_MEMORY_H

#ifdef __cplusplus
  extern "C" {
#endif


#include <stddef.h>
typedef void *  (*MallocHookType)(size_t);
typedef void  (*FreeHookType)(void*);


JPEGEXPORT
void * FPX_malloc(size_t);
JPEGEXPORT
void * FPX_calloc(size_t, size_t);
JPEGEXPORT
void FPX_free(void *);
JPEGEXPORT
void FPXSetMemoryHooks(MallocHookType,FreeHookType);

#ifdef __cplusplus
  }
#endif

#endif
