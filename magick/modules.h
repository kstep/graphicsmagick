/*
  ImageMagick Modules Methods.
*/
#ifndef _MODULES_H
#define _MODULES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(HasLTDL)
# include "ltdl/ltdl.h"
  typedef lt_dlhandle ModuleHandle;
#endif /* HasLTDL */

typedef struct _ModuleInfo
{
  char
    *tag;

  ModuleHandle
    handle;

  time_t
    load_time;

  struct _ModuleInfo
    *previous,
    *next;
} ModuleInfo;

  /*
    Modules declarations.
  */
extern Export void
  DestroyModuleInfo(),
  ExitModules(void),
  InitializeModules(void);

extern Export int
  LoadModule(const char* module),
  UnregisterModuleInfo(const char *tag),
  UnloadModule(const char* module);

extern Export char
  **ListModules(void);

extern Export ModuleInfo
  *GetModuleInfo(const char *tag),
  *RegisterModuleInfo(ModuleInfo *entry),
  *SetModuleInfo(const char *tag);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
