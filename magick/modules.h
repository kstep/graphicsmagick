/*
  ImageMagick Modules Methods.
*/
#ifndef _MODULES_H
#define _MODULES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(HasLTDL)
#include "ltdl/ltdl.h"
typedef lt_dlhandle ModuleHandle;
#else
typedef void *ModuleHandle;
#endif

typedef struct _ModuleInfo
{
  char
    *tag;

  ModuleHandle
    handle;

  time_t
    load_time;

  unsigned long
    signature;

  struct _ModuleInfo
    *previous,
    *next;
} ModuleInfo;

/*
  Modules declarations.
*/

extern MagickExport ModuleInfo
  *GetModuleInfo(const char *,ExceptionInfo *);

extern MagickExport unsigned int
  ExecuteModuleProcess(const char *,Image *,const int,char **),
  OpenModule(const char *,ExceptionInfo *),
  OpenModules(ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
