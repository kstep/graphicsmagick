/*
  ImageMagick Utility Methods.
*/
#ifndef _UTILITY_H
#define _UTILITY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if HAVE_SYS_NDIR_H || HAVE_SYS_DIR_H || HAVE_NDIR_H
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#else
# include <dirent.h>
# define NAMLEN(dirent) Extent((dirent)->d_name)
#endif
#include <pwd.h>
#else
#if defined(vms)
#include "vms.h"
#endif
#if defined(macintosh)
#include "mac.h"
#endif
#if defined(WIN32)
#include "nt.h"
#endif
#endif
#if defined(WITH_DMALLOC)
#include <dmalloc.h>
#endif

#if !defined(S_ISDIR)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

/*
  Typedef declarations.
*/
typedef struct _TokenInfo
{
  int
    state;  /* current state */

  unsigned
    flag;  /* option flag */

  char
    quote;  /* current quote char */

  int
    offset;  /* current token pos */
} TokenInfo;

/*
  Utilities methods.
*/
extern MagickExport char
  *AllocateString(const char *),
  *BaseFilename(const char *),
  **ListColors(const char *,int *),
  **ListFiles(const char *,const char *,int *),
  *PostscriptGeometry(const char *),
  *SetClientName(const char *),
  *SetClientPath(const char *),
  **StringToArgv(const char *,int *),
  **StringToList(const char *),
  *TranslateText(const ImageInfo *,Image *,const char *);

extern MagickExport int
  GetGeometry(const char *,int *,int *,unsigned int *,unsigned int *),
  GlobExpression(const char *,const char *),
  LocaleNCompare(const char *,const char *,size_t),
  LocaleCompare(const char *,const char *),
  MultilineCensus(const char *),
  SystemCommand(const unsigned int,const char *),
  Tokenizer(TokenInfo *,unsigned,char *,int,char *,char *,char *,char *,
    char,char *,int *,char *);

extern MagickExport unsigned int
  CGIToArgv(const char *,int *,char ***),
  CloneString(char **,const char *),
  ConcatenateString(char **,const char *),
  ExpandFilenames(int *,char ***),
  IsAccessible(const char *),
  IsDirectory(const char *);

extern MagickExport unsigned short
  *ConvertTextToUnicode(const char *,int *);

extern MagickExport void
  *AllocateMemory(const size_t),
  AppendImageFormat(const char *,char *),
  DestroyPostscriptGeometry(char *),
  ExpandFilename(char *),
  FreeMemory(void **),
  FormatString(char *,const char *,...),
  LocaleLower(char *),
  LocaleUpper(char *),
  LocaleFilename(char *),
  ReallocateMemory(void **,const size_t),
  Strip(char *),
  TemporaryFilename(char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
