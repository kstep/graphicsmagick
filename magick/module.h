/*
  ImageMagick Modules Methods.
*/
#ifndef _MAGICK_MODULE_H
#define _MAGICK_MODULE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _ModuleAlias
{
  char
    *filename,
    *name,
    *alias;

  struct _ModuleAlias
    *previous,
    *next;
} ModuleAlias;

typedef struct _ModuleInfo
{
  char
    *tag;
  
  void
    *handle;
  
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
extern MagickExport ModuleAlias
  *GetModuleAlias(const char *,ExceptionInfo *);

extern MagickExport ModuleInfo
  *GetModuleInfo(const char *,ExceptionInfo *);

extern MagickExport unsigned int
  ExecuteModuleProcess(const char *,Image *,const int,char **),
  ListModuleAliases(FILE *,ExceptionInfo *),
  OpenModule(const char *,ExceptionInfo *),
  OpenModules(ExceptionInfo *);

extern MagickExport void
  DestroyModuleInfo(void);

extern void
  OpenStaticModules(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
