/*
  ImageMagick Quantization declarations.
*/
#ifndef _QUANTIZE_H
#define _QUANTIZE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declarations.
*/
#define CacheShift  (QuantumDepth-6)
#define ErrorQueueLength  16
#define MaxNodes  266817
#define MaxSpan ((1 << MaxTreeDepth)-1)
#define MaxTreeDepth  8
#define NodesInAList  2048

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
