/*
  Methods to Reduce the Number of Unique Colors in an Image.
*/
#ifndef _QUANTIZE_H
#define _QUANTIZE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _ErrorPacket
{
  int
    red,
    green,
    blue;
} ErrorPacket;

typedef struct _QuantizeInfo
{
  unsigned int
    number_colors,
    tree_depth,
    dither;

  ColorspaceType
    colorspace;
} QuantizeInfo;

/*
  Quantization utilities methods.
*/
extern Export QuantizeInfo
  *CloneQuantizeInfo(const QuantizeInfo *);

extern Export unsigned int
  MapImage(Image *,Image *,const unsigned int),
  MapImages(Image *,Image *,const unsigned int),
  QuantizationError(Image *),
  QuantizeImage(QuantizeInfo *,Image *),
  QuantizeImages(QuantizeInfo *,Image *),
  QueryColorName(const ColorPacket *,char *),
  SegmentImage(Image *,const ColorspaceType,const unsigned int,const double,
    const double);

extern Export void
  DestroyQuantizeInfo(QuantizeInfo *),
  GetQuantizeInfo(QuantizeInfo *),
  NumberColors(Image *,FILE *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
