/*
  ImageMagick Modules Methods.
*/
#ifndef _MODULES_H
#define _MODULES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declarations.
*/
#if defined(HasLTDL)
#include "ltdl/ltdl.h"
typedef lt_dlhandle ModuleHandle;
#endif
#if defined(_MAGICKMOD_)
typedef void *ModuleHandle;
#if defined(_MT) && defined(_VISUALC_)
#  define CoderModuleDirectory "."
#if defined(_DEBUG)
#  define ModuleSearchSpec "IM_MOD_DB_*.dll"
#else
#  define ModuleSearchSpec "IM_MOD_RL_*.dll"
#endif
#endif
#endif

/*
  Typedef declarations.
*/
typedef struct _ModuleAliases
{
  char
    *alias,
    *module;

  struct _ModuleAliases
    *next;
} ModuleAliases;

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
extern Export char
  **ListModules(void),
  *TagToModule(const char *);

extern Export int
  LoadAllModules(void),
  LoadDynamicModule(const char *),
  UnloadDynamicModule(const char *),
  UnregisterModuleInfo(const char *);

extern Export ModuleInfo
  *GetModuleInfo(const char *),
  *RegisterModuleInfo(ModuleInfo *),
  *SetModuleInfo(const char *);

extern Export unsigned int
  CallImageFilter(const char *,Image *,const char *);

extern Export void
  DestroyModuleInfo(void),
  ExitModules(void),
  InitializeModules(void),
  ModuleToTag(const char *,const char *,char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
