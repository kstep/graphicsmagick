/*
  ImageMagick Magic Methods.
*/
#ifndef _MAGIC_H
#define _MAGIC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declaration.
*/
#define MagicInfoListExtent 256
#define StringMethodArgumentExtent  64

/*
  Typedef declaractions.
*/
typedef enum
{
  UndefinedMagicMethod,
  StringMagicMethod
} MagicMethod;

typedef struct _StringMethodArgument
{
  unsigned char
    value[StringMethodArgumentExtent];

  unsigned int
    length,
    offset;
} StringMethodArgument;

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
extern MagickExport unsigned int
  SetImageMagic(const unsigned char *,const unsigned int,char *);

extern MagickExport void
  DestroyMagicInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
