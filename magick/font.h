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

typedef struct _FontMetric
{
  PointInfo
    pixels_per_em;

  int
    ascent,
    descent;

  unsigned int
    width,
    height,
    max_advance;
} FontMetric;

/*
  Method declarations.
*/
extern MagickExport FontInfo
  *GetFontInfo(const char *,ExceptionInfo *);

extern MagickExport unsigned int
  GetFontMetrics(Image *,const DrawInfo *,FontMetric *),
  ListFontInfo(FILE *,ExceptionInfo *);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
