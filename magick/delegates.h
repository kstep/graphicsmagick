/*
  Delegate Application Programming Interface declarations.
*/
#ifndef _DELEGATES_H
#define _DELEGATES_H

/*
  Delegate define definitions.
*/
#if !defined(WIN32)
#define DelegatePath  "/usr/local/share"
#else
#define DelegatePath  "c:"
#endif
#define DelegateFilename  "delegates.mgk"

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
  Exported delegate methods.
*/
Export char
  *GetDelegateCommand(const ImageInfo *,Image *,char *,unsigned int);

Export DelegateInfo
  *SetDelegateInfo(DelegateInfo *);

Export unsigned int
  GetDelegateInfo(char *,unsigned int,DelegateInfo *),
  InvokeDelegate(const ImageInfo *,Image *,char *,unsigned int);
#endif
