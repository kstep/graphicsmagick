/*
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
  struct _DelegateInfo
    *previous,
    *next;

  char
    *filename,
    *decode,
    *encode,
    *commands;

  int
    mode;

  unsigned int
    restrain,
    spawn;
} DelegateInfo;

/*
  Magick delegate methods.
*/
extern MagickExport char
  *GetDelegateCommand(const ImageInfo *,Image *,const char *,const char *,
    ExceptionInfo *);

extern MagickExport DelegateInfo
  *GetDelegateInfo(const char *,const char *,ExceptionInfo *exception),
  *SetDelegateInfo(DelegateInfo *);

extern MagickExport unsigned int
  InvokeDelegate(const ImageInfo *,Image *,const char *,const char *,
    ExceptionInfo *),
  ListDelegateInfo(FILE *,ExceptionInfo *);

extern MagickExport void
  DestroyDelegateInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
