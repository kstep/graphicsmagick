/*
  Methods to Read/Write/Invoke Delegates.
*/
#ifndef _DELEGATES_H
#define _DELEGATES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Delegate structure definitions.
*/
typedef struct _DelegateInfo
{
  char
    decode_tag[MaxTextExtent],
    encode_tag[MaxTextExtent],
    *commands;

  int
    direction;

  struct _DelegateInfo
    *previous,
    *next;
} DelegateInfo;

/*
  MagickExported delegate methods.
*/
extern MagickExport char
  *GetDelegateCommand(const ImageInfo *,Image *,const char *,const char *);

extern MagickExport DelegateInfo
  *SetDelegateInfo(DelegateInfo *);

extern MagickExport unsigned int
  GetDelegateInfo(const char *,const char *,DelegateInfo *),
  InvokeDelegate(const ImageInfo *,Image *,const char *,const char *),
  ListDelegateInfo(FILE *);

extern MagickExport void
  DestroyDelegateInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
