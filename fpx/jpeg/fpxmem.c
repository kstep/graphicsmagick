/*

  Temporary implementation of a memory manager in the 
  JPEG library used in FlashPix.


  We have define the operators FPXMalloc and FPXFree


  */


#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "jpegconf.h"
#include "fpxmem.h"

MallocHookType  fpxMallocHook = 0;
FreeHookType  fpxFreeHook = 0;

void * FPX_malloc(size_t theSize)
{
  return (fpxMallocHook ? (*fpxMallocHook)(theSize) : malloc(theSize));
}

void * FPX_calloc(size_t num, size_t size)
{
  size_t realsize = num*size;
  void * ptr;
  if ((ptr = FPX_malloc(realsize)))
    memset(ptr,0,realsize);
  return ptr;
}



void FPX_free(void * ptr)
{
  if (fpxFreeHook)
    (*fpxFreeHook)(ptr);
  else
    free(ptr);
}

void FPXSetMemoryHooks(MallocHookType mallocHook, FreeHookType freeHook)
{
  if ( (mallocHook) && (freeHook)) {
    fpxMallocHook = mallocHook;
    fpxFreeHook   = freeHook;
  }
  else {
    fpxMallocHook = 0;
    fpxFreeHook   = 0;
  }
}


