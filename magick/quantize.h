/*
  Methods to Reduce the Number of Unique Colors in an Image.
*/
#ifndef _MAGICK_QUANTIZE_H
#define _MAGICK_QUANTIZE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declarations.
*/
#define MaxTreeDepth  8
#define NodesInAList  1536

/*
  Typedef declarations.
*/
typedef struct _QuantizeInfo
{
  unsigned long
    signature;

  unsigned long
    number_colors;

  unsigned int
    tree_depth,
    dither;

  ColorspaceType
    colorspace;

  unsigned int
    measure_error;
} QuantizeInfo;

/*
  Quantization utilities methods.
*/
extern MagickExport QuantizeInfo
  *CloneQuantizeInfo(const QuantizeInfo *);

extern MagickExport unsigned int
  MapImage(Image *,const Image *,const unsigned int),
  MapImages(Image *,const Image *,const unsigned int),
  OrderedDitherImage(Image *),
  QuantizationError(Image *),
  QuantizeImage(const QuantizeInfo *,Image *),
  QuantizeImages(const QuantizeInfo *,Image *),
  SegmentImage(Image *,const ColorspaceType,const unsigned int,const double,
    const double);

extern MagickExport void
  CompressColormap(Image *),
  DestroyQuantizeInfo(QuantizeInfo *),
  GetQuantizeInfo(QuantizeInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
