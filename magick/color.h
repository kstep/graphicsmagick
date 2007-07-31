/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Color Methods.
*/
#ifndef _MAGICK_COLOR_H
#define _MAGICK_COLOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#if defined(MAGICK_IMPLEMENTATION)

#define VerifyColormapIndex(image,index) \
{ \
  if (index >= image->colors) \
    { \
      char \
        colormapIndexBuffer[MaxTextExtent]; \
      \
      FormatString(colormapIndexBuffer,"index %u >= %u colors, %.1024s", \
        (unsigned int) index, image->colors, image->filename); \
      errno=0; \
      index=0U; \
      ThrowException(&image->exception,CorruptImageError, \
        InvalidColormapIndex,colormapIndexBuffer); \
    } \
}

/*
  Macros for testing a pixel to see if it is grayscale or bilevel.
*/
#define IsGray(color)  \
  (((color).red == (color).green) && ((color).green == (color).blue))
#define IsMonochrome(color) \
  (IsGray(color) && ((0 == (color).red) || (MaxRGB == (color).red)))

/*
  Compare two colors
*/
#define ColorMatch(p,q) (((p)->red == (q)->red) && \
  ((p)->green == (q)->green) && ((p)->blue == (q)->blue))

#endif /* defined(MAGICK_IMPLEMENTATION) */

/*
  PixelPacket with usage count, used to support color histograms.
*/
typedef struct _HistogramColorPacket
{
  PixelPacket
    pixel;

  unsigned long
    count;
} HistogramColorPacket;

extern MagickExport char
  **GetColorList(const char *,unsigned long *);

extern MagickExport const ColorInfo
  *GetColorInfo(const char *,ExceptionInfo *);

extern MagickExport ColorInfo
  **GetColorInfoArray(ExceptionInfo *exception);

extern MagickExport HistogramColorPacket
  *GetColorHistogram(const Image *, unsigned long *colors, ExceptionInfo *);

extern MagickExport unsigned int
  FuzzyColorMatch(const PixelPacket *,const PixelPacket *,const double),
  IsGrayImage(const Image *,ExceptionInfo *),
  IsMonochromeImage(const Image *,ExceptionInfo *),
  IsOpaqueImage(const Image *,ExceptionInfo *),
  IsPaletteImage(const Image *,ExceptionInfo *),
  ListColorInfo(FILE *,ExceptionInfo *),
  QueryColorDatabase(const char *,PixelPacket *,ExceptionInfo *),
  QueryColorname(const Image *,const PixelPacket *,const ComplianceType,char *,
    ExceptionInfo *);

extern MagickExport unsigned long
  GetNumberColors(const Image *,FILE *,ExceptionInfo *);

extern MagickExport void
  DestroyColorInfo(void),
  GetColorTuple(const PixelPacket *,const unsigned int,const unsigned int,
    const unsigned int,char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_COLOR_H */
