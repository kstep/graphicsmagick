/*
  ImageMagick Modules Methods.
*/
#ifndef _MAGICK_MODULE_H
#define _MAGICK_MODULE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _CoderInfo
{
  char
    *tag;
  
  void
    *handle;
  
  time_t
    load_time;

  unsigned long
    signature;

  struct _CoderInfo
    *previous,
    *next;
} CoderInfo;

typedef struct _ModuleInfo
{
  char
    *filename,
    *magick,
    *name;

  unsigned int
    stealth;

  unsigned long
    signature;

  struct _ModuleInfo
    *previous,
    *next;
} ModuleInfo;

/*
  Modules declarations.
*/
extern MagickExport char
  *TagToModule(const char *);

extern MagickExport const CoderInfo
  *GetCoderInfo(const char *,ExceptionInfo *);

extern MagickExport const ModuleInfo
  *GetModuleInfo(const char *,ExceptionInfo *);

extern MagickExport unsigned int
  ExecuteModuleProcess(const char *,Image **,const int,char **),
  ListModuleInfo(FILE *,ExceptionInfo *),
  OpenModule(const char *,ExceptionInfo *),
  OpenModules(ExceptionInfo *);

extern unsigned int
  ExecuteStaticModuleProcess(const char *,Image **,const int,char **);

extern MagickExport void
  DestroyModuleInfo(void);

extern void
  OpenStaticModules(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
