/*
  Color methods.
*/
#ifndef _MAGICK_COLORS_H
#define _MAGICK_COLORS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _ColorInfo
{  
  char
    *filename,
    *name;

  ComplianceType
    compliance;

  PixelPacket
    color;

  struct _ColorInfo
    *previous,
    *next;
} ColorInfo;

/*
  Method declarations.
*/
extern MagickExport IndexPacket
  ValidateColormapIndex(Image *,const int);

extern MagickExport char
  **GetColorList(const char *,int *);

extern MagickExport ColorInfo
  *GetColorInfo(const char *,ExceptionInfo *);

extern MagickExport size_t
  GetNumberColors(Image *,FILE *);

extern MagickExport unsigned int
  IsGrayImage(Image *),
  IsMonochromeImage(Image *),
  IsOpaqueImage(Image *),
  IsPseudoClass(Image *),
  ListColorInfo(FILE *,ExceptionInfo *),
  QueryColorDatabase(const char *,PixelPacket *),
  QueryColorname(Image *,const PixelPacket *,const ComplianceType,char *);

extern MagickExport void
  DestroyColorInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
