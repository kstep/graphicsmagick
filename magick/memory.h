
#if defined(WITH_DMALLOC)
#include <dmalloc.h>
#endif
/*
  Memory declarations.
*/
extern Export void
  *AllocateMemory(size_t),
  FreeMemory(void *),
  *ReallocateMemory(void *,size_t);
