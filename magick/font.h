/*
  Drawing methods.
*/
#ifndef _FONT_H
#define _FONT_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _FontInfo
{  
  char
    *name,
    *family,
    *alias,
    *description,
    *format,
    *weight,
    *glyphs,
    *metrics,
    *version;

  struct _FontInfo
    *previous,
    *next;
} FontInfo;

/*
  Method declarations.
*/
extern MagickExport FontInfo
  *GetFontInfo(const char *,ExceptionInfo *);

extern MagickExport unsigned int
  ListFontInfo(FILE *,ExceptionInfo *);

extern MagickExport void
  DestroyFontInfo(DrawInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
