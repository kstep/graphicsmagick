/*
  Magic methods.
*/
#ifndef _MAGIC_H
#define _MAGIC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _MagicInfo
{  
  char
    *name,
    *target;

  unsigned int
    offset;

  struct _MagicInfo
    *previous,
    *next;
} MagicInfo;

/*
  Method declarations.
*/
extern MagickExport MagicInfo
  *GetMagicInfo(const unsigned char *,const unsigned int,ExceptionInfo *);

extern MagickExport unsigned int
  ListMagicInfo(FILE *,ExceptionInfo *);

extern MagickExport void
  DestroyMagicInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
