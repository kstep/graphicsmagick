/*
  ImageMagick Magic Methods.
*/
#ifndef _MAGIC_H
#define _MAGIC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declaractions.
*/
typedef struct _MagicInfoMember
{
  MagicMethod
    method;

  void
    *argument;

  int
    status;

  struct _MagicInfoMember
    *next;
} MagicInfoMember;

typedef struct _MagicInfo
{
  char
    *tag;

  struct _MagicInfoMember
    *member;
} MagicInfo;

/*
  Magic method declarations.
*/
extern MagickExport MagicInfo
  *GetMagicInfo(const unsigned char *,const unsigned int,ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
