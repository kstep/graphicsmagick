
#if defined(WITH_DMALLOC)
#include <dmalloc.h>
#endif
/*
  Memory declarations.
*/
extern Export void
  *AllocateMemory(const size_t),
  FreeMemory(void *),
  *ReallocateMemory(void *,const size_t);
