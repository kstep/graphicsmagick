/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Methods to Read/Write/Invoke Delegates.
*/
#ifndef _MAGICK_DELEGATE_H
#define _MAGICK_DELEGATE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Delegate structure definitions.
*/
typedef struct _DelegateInfo
{
  char
    *path,
    *decode,
    *encode;

  char
    *commands;

  int
    mode;

  unsigned int
    stealth,
    spawn;

  unsigned long
    signature;

  struct _DelegateInfo
    *previous,
    *next;
} DelegateInfo;

#if defined(MAGICK_IMPLEMENTATION)

#if defined(HasGS)
#include "ps/iapi.h"
#include "ps/errors.h"
#endif

#ifndef gs_main_instance_DEFINED
# define gs_main_instance_DEFINED
typedef struct gs_main_instance_s gs_main_instance;
#endif

#if !defined(MagickDLLCall)
#  if defined(WIN32)
#    define MagickDLLCall __stdcall
#  else
#    define MagickDLLCall
#  endif
#endif

/*
  Define a vector of Ghostscript library callback functions so that
  DLL/shared and static Ghostscript libbraries may be handled identically.
  These definitions must be compatible with those in the Ghostscript API
  headers (which we don't require).
  */
typedef struct _GhostscriptVectors
{
  int  (MagickDLLCall *exit)(gs_main_instance *);
  void (MagickDLLCall *delete_instance)(gs_main_instance *);
  int  (MagickDLLCall *init_with_args)(gs_main_instance *,int,char **);
  int  (MagickDLLCall *new_instance)(gs_main_instance **,void *);
  int  (MagickDLLCall *run_string)(gs_main_instance *,const char *,int,int *);
} GhostscriptVectors;
#endif /* MAGICK_IMPLEMENTATION */

/*
  Magick delegate methods.
*/
extern MagickExport char
  *GetDelegateCommand(const ImageInfo *,Image *,const char *,const char *,
    ExceptionInfo *);

extern MagickExport const DelegateInfo
  *GetDelegateInfo(const char *,const char *,ExceptionInfo *exception);

extern MagickExport DelegateInfo
  *SetDelegateInfo(DelegateInfo *);

extern MagickExport unsigned int
  InvokePostscriptDelegate(const unsigned int,const char *),
  InvokeDelegate(ImageInfo *,Image *,const char *,const char *,ExceptionInfo *),
  ListDelegateInfo(FILE *,ExceptionInfo *);

extern MagickExport void
  DestroyDelegateInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
