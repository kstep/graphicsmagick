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

  unsigned long
    signature;

  struct _DelegateInfo
    *previous,
    *next;
} DelegateInfo;

/*
  Magick delegate methods.
*/
extern MagickExport char
  *GetDelegateCommand(const ImageInfo *,Image *,const char *,const char *);

extern MagickExport DelegateInfo
  *GetDelegateInfo(const char *,const char *,ExceptionInfo *exception),
  *SetDelegateInfo(DelegateInfo *);

extern MagickExport unsigned int
  InvokeDelegate(const ImageInfo *,Image *,const char *,const char *),
  ListDelegateInfo(FILE *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
