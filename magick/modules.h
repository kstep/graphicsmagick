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
#if !defined(_VISUALC_)
#if !defined(CoderModuleDirectory)
#define CoderModuleDirectory  ""
#endif
#define ModuleGlobExpression "*.so"
#else
#if defined(_DEBUG)
#define ModuleGlobExpression "IM_MOD_DB_*.dll"
#else
#define ModuleGlobExpression "IM_MOD_RL_*.dll"
#endif
#endif

/*
  Typedef declarations.
*/
#if defined(HasLTDL)
#include "ltdl/ltdl.h"
typedef lt_dlhandle ModuleHandle;
#else
typedef void *ModuleHandle;
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

  unsigned int
    signature;

  struct _ModuleInfo
    *previous,
    *next;
} ModuleInfo;

/*
  Modules declarations.
*/
extern MagickExport char
  **ListModules(void),
  *TagToModule(const char *);

extern MagickExport int
  OpenModules(void),
  OpenModule(const char *),
  UnloadDynamicModule(const char *),
  UnregisterModuleInfo(const char *);

extern MagickExport ModuleInfo
  *GetModuleInfo(const char *),
  *RegisterModuleInfo(ModuleInfo *),
  *SetModuleInfo(const char *);

extern MagickExport unsigned int
  ExecuteModuleProcess(const char *,Image *,const int,char **);

extern MagickExport void
  DestroyModuleInfo(void),
  ExitModules(void),
  InitializeModules(void),
  ModuleToTag(const char *,const char *,char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
