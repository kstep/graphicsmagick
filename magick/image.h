/*
  ImageMagick Image Methods.
*/
#ifndef _IMAGE_H
#define _IMAGE_H

#if defined(QuantumLeap)
/*
  Color quantum is [0..65535].
*/
#define DownScale(quantum)  ((quantum)/256)
#define HexColorFormat "#%04x%04x%04x"
#define MaxRGB  65535L
#define QuantumDepth  16
#define UpScale(quantum)  (257*(quantum))
#define XDownScale(color)  (color)
#define XUpScale(color)  (color)

typedef unsigned short Quantum;
#else
/*
  Color quantum is [0..255].
*/
#define DownScale(quantum)  (quantum)
#define HexColorFormat "#%02x%02x%02x"
#define MaxRGB  255L
#define QuantumDepth  8
#define UpScale(quantum)  (quantum)
#define XDownScale(color)  ((color)/256)
#define XUpScale(color)  (257*(color))

typedef unsigned char Quantum;
#endif
#define OpaqueOpacity  0
#define TransparentOpacity  MaxRGB

/*
  3D effects.
*/
#define AccentuateModulate  UpScale(80)
#define HighlightModulate  UpScale(125)
#define ShadowModulate  UpScale(135)
#define DepthModulate  UpScale(185)
#define TroughModulate  UpScale(110)

/*
  Typedef declarations.
*/
typedef struct _AffineInfo
{
  double
    sx,
    rx,
    ry,
    sy,
    tx,
    ty;
} AffineInfo;

typedef struct _Ascii85Info
{
  int
    offset,
    line_break;

  unsigned char
    buffer[10];
} Ascii85Info;

typedef struct _PointInfo
{
  double
    x,
    y,
    z;
} PointInfo;

typedef struct _PixelPacket
{
#if defined(WORDS_BIGENDIAN)
  Quantum
    red,
    green,
    blue,
    opacity;
#else
  Quantum
    blue,
    green,
    red,
    opacity;
#endif
} PixelPacket;

typedef struct _RectangleInfo
{
  unsigned int
    width,
    height;

  int
    x,
    y;
} RectangleInfo;

typedef struct _BoundingBox
{
  double
    width,
    height,
    x,
    y;
} BoundingBox;

typedef struct _BlobInfo
{
  unsigned int
    mapped;

  char
    *data;

  off_t
    offset;

  size_t
    length,
    extent,
    quantum;

  unsigned long
    signature;
} BlobInfo;

typedef struct _ChromaticityInfo
{
  PointInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

typedef struct _ExceptionInfo
{
  ExceptionType
    severity;

  char
    *reason,
    *description;

  unsigned long
    signature;
} ExceptionInfo;

typedef struct _FrameInfo
{
  unsigned int
    width,
    height;

  int
    x,
    y,
    inner_bevel,
    outer_bevel;
} FrameInfo;

typedef struct _ImageAttribute
{
  char
    *key,
    *value;

  unsigned int
    compression;

  struct _ImageAttribute
    *previous,
    *next;
} ImageAttribute;

typedef Quantum IndexPacket;

typedef struct _MontageInfo
{
  char
    filename[MaxTextExtent],
    *geometry,
    *tile,
    *title,
    *frame,
    *texture,
    *font;

  double
    pointsize;

  GravityType
    gravity;

  unsigned int
    border_width,
    shadow;

  CompositeOperator
    compose;

  PixelPacket
    fill,
    stroke,
    background_color,
    border_color,
    matte_color;

  unsigned long
    signature;
} MontageInfo;

typedef struct _ProfileInfo
{
  unsigned int
    length;

  char
    *name;

  unsigned char
    *info;
} ProfileInfo;

typedef struct _SegmentInfo
{
  double
    x1,
    y1,
    x2,
    y2;
} SegmentInfo;

typedef struct _Timer
{
  double
    start,
    stop,
    total;
} Timer;

typedef struct _TimerInfo
{
  Timer
    user,
    elapsed;

  TimerState
    state;

  unsigned long
    signature;
} TimerInfo;

typedef struct _Image
{
  int
    exempt,
    status,
    temporary;

  char
    filename[MaxTextExtent];

  FILE
    *file;

  size_t
    filesize;

  int
    pipe;

  char
    magick[MaxTextExtent];

  ClassType
    storage_class;

  unsigned int
    matte;

  CompressionType
    compression;

  unsigned int
    columns,
    rows,
    depth;

  int
    offset;

  RectangleInfo
    tile_info;

  InterlaceType
    interlace;

  unsigned int
    scene;

  char
    *montage,
    *directory;

  PixelPacket
    *colormap;

  unsigned int
    colors;

  ColorspaceType
    colorspace;

  RenderingIntent
    rendering_intent;

  double
    gamma;

  ChromaticityInfo
    chromaticity;

  ProfileInfo
    color_profile,
    iptc_profile;

  unsigned int
    generic_profiles;

  ProfileInfo
    *generic_profile;

  ResolutionType
    units;

  double
    x_resolution,
    y_resolution;

  PixelPacket
    background_color,
    border_color,
    matte_color;

  char
    *geometry;

  RectangleInfo
    page;

  unsigned int
    dispose,
    delay,
    iterations;

  double
    fuzz;

  FilterTypes
    filter;

  double
    blur;

  unsigned long
    total_colors;

  unsigned int
    mean_error_per_pixel;

  double
    normalized_mean_error,
    normalized_maximum_error;

  char
    magick_filename[MaxTextExtent];

  unsigned int
    magick_columns,
    magick_rows;

  int
    restart_animation_here,
    taint;

  TimerInfo
    timer;

  ExceptionInfo
    exception;

  Ascii85Info
    ascii85;

  BlobInfo
    blob;

  ImageAttribute
    *attributes;

  void
    *cache;

  int
    (*fifo)(const struct _Image *,const void *,const size_t);

  void
    *client_data;

  unsigned int
    orphan;

  struct _Image
    *previous,
    *list,
    *next;

  unsigned long
    signature;
} Image;

typedef struct _ImageInfo
{
  /*
    Blob member.
  */
  BlobInfo
    blob;

  /*
    File and image dimension members.
  */
  FILE
    *file;

  char
    filename[MaxTextExtent],
    magick[MaxTextExtent],
    unique[MaxTextExtent],
    zero[MaxTextExtent];

  unsigned int
    temporary,
    adjoin,
    subimage,
    subrange,
    depth;

  char
    *size,
    *tile,
    *page;

  InterlaceType
    interlace;

  ResolutionType
    units;

  /*
    Compression members.
  */
  CompressionType
    compression;

  unsigned int
    quality;

  /*
    Annotation members.
  */
  char
    *server_name,
    *font,
    *texture,
    *density;

  unsigned int
    antialias;

  double
    pointsize;

  AffineInfo
    affine;

  double
    fuzz;

  PixelPacket
    fill,
    stroke,
    background_color,
    border_color,
    matte_color;

  /*
    Color reduction members.
  */
  unsigned int
    dither,
    monochrome;

  ColorspaceType
    colorspace;

  /*
    Animation members.
  */
  char
    *dispose,
    *delay,
    *iterations;

  unsigned int
    decode_all_MNG_objects,
    coalesce_frames,
    insert_backdrops;

  /*
    Miscellaneous members.
  */
  unsigned int
    verbose;

  PreviewType
    preview_type;

  char
    *view;

  long
    group;

  unsigned int
    ping;

  int
    (*fifo)(const Image *,const void *,const size_t);

  void
    *client_data;

  unsigned long
    signature;
} ImageInfo;

typedef struct _MagickInfo
{
  char
    *tag;

  ImageInfo
    *image_info;

  Image
    *(*decoder)(const ImageInfo *,ExceptionInfo *);

  unsigned int
    (*encoder)(const ImageInfo *,Image *),
    (*magick)(const unsigned char *,const unsigned int),
    adjoin,
    blob_support,
    raw;

  char
    *description,
    *module;

  void
    *client_data;

  struct _MagickInfo
    *previous,
    *next;

  unsigned long
    signature;
} MagickInfo;

/*
  Image const declarations.
*/
extern MagickExport const char
  *Alphabet,
  *AppendBinaryType,
  *BackgroundColor,
  *BorderColor,
  *DefaultPointSize,
  *DefaultTileFrame,
  *DefaultTileGeometry,
  *DefaultTileLabel,
  *ForegroundColor,
  *MatteColor,
  *LoadImageText,
  *LoadImagesText,
  *PSDensityGeometry,
  *PSPageGeometry,
  *ReadBinaryType,
  *ReadBinaryUnbufferedType,
  *SaveImageText,
  *SaveImagesText,
  *WriteBinaryType;

/*
  Image utilities methods.
*/
extern MagickExport char 
  *GetImageMagick(const unsigned char *,const unsigned int),
  *GetMagickConfigurePath(const char *);

extern MagickExport Image
  *AddNoiseImage(Image *,const NoiseType,ExceptionInfo *),
  *AllocateImage(const ImageInfo *),
  *AppendImages(Image *,const unsigned int,ExceptionInfo *),
  *AverageImages(Image *,ExceptionInfo *),
  *BlurImage(Image *,const double,const double,ExceptionInfo *),
  *BorderImage(Image *,const RectangleInfo *,ExceptionInfo *),
  *ChopImage(Image *,const RectangleInfo *,ExceptionInfo *),
  *CloneImage(Image *,const unsigned int,const unsigned int,const unsigned int,
    ExceptionInfo *),
  *CoalesceImages(Image *,ExceptionInfo *),
  *ColorizeImage(Image *,const char *,const PixelPacket,ExceptionInfo *),
  *ConstituteImage(const unsigned int,const unsigned int,const char *,
    const StorageType,const void *,ExceptionInfo *),
  *ConvolveImage(Image *,const unsigned int,const double *,ExceptionInfo *),
  *CropImage(Image *,const RectangleInfo *,ExceptionInfo *),
  *DeconstructImages(Image *,ExceptionInfo *),
  *DespeckleImage(Image *,ExceptionInfo *),
  *EdgeImage(Image *,const double,ExceptionInfo *),
  *EmbossImage(Image *,const double,const double,ExceptionInfo *),
  *EnhanceImage(Image *,ExceptionInfo *),
  *FlipImage(Image *,ExceptionInfo *),
  *FlopImage(Image *,ExceptionInfo *),
  *FrameImage(Image *,const FrameInfo *,ExceptionInfo *),
  *GetNextImage(Image *),
  *GaussianBlurImage(Image *,const double,const double,ExceptionInfo *),
  *ImplodeImage(Image *,const double,ExceptionInfo *),
  **ListToGroupImage(Image *,unsigned int *),
  *MagnifyImage(Image *,ExceptionInfo *),
  *MedianFilterImage(Image *,const double,ExceptionInfo *),
  *MinifyImage(Image *,ExceptionInfo *),
  *MontageImages(Image *,const MontageInfo *,ExceptionInfo *),
  *MorphImages(Image *,const unsigned int,ExceptionInfo *),
  *MosaicImages(Image *,ExceptionInfo *),
  *OilPaintImage(Image *,const double,ExceptionInfo *),
  *PingImage(const ImageInfo *,ExceptionInfo *),
  *ReadImage(const ImageInfo *,ExceptionInfo *),
  *ReadImages(const ImageInfo *,ExceptionInfo *),
  *ReduceNoiseImage(Image *,const double,ExceptionInfo *),
  *ResizeImage(Image *,const unsigned int,const unsigned int,const FilterTypes,
    const double,ExceptionInfo *),
  *RollImage(Image *,const int,const int,ExceptionInfo *),
  *RotateImage(Image *,const double,ExceptionInfo *),
  *SampleImage(Image *,const unsigned int,const unsigned int,ExceptionInfo *),
  *ScaleImage(Image *,const unsigned int,const unsigned int,ExceptionInfo *),
  *ShadeImage(Image *,const unsigned int,double,double,ExceptionInfo *),
  *SharpenImage(Image *,const double,const double,ExceptionInfo *),
  *ShearImage(Image *,const double,const double,ExceptionInfo *),
  *SpreadImage(Image *,const unsigned int,ExceptionInfo *),
  *SteganoImage(Image *,Image *,ExceptionInfo *),
  *StereoImage(Image *,Image *,ExceptionInfo *),
  *SwirlImage(Image *,double,ExceptionInfo *),
  *UnsharpMaskImage(Image *,const double,const double,const double,const double,
    ExceptionInfo *),
  *WaveImage(Image *,const double,const double,ExceptionInfo *),
  *ZoomImage(Image *,const unsigned int,const unsigned int,ExceptionInfo *);

extern MagickExport ImageInfo
  *CloneImageInfo(const ImageInfo *);

extern MagickExport ImageType
  GetImageType(Image *);

extern MagickExport IndexPacket
  *(*GetIndexes)(const Image *);

extern MagickExport int
  ParseGeometry(const char *,int *,int *,unsigned int *,unsigned int *),
  ParseImageGeometry(const char *,int *,int *,unsigned int *,unsigned int *);

extern MagickExport MagickInfo
  *GetMagickInfo(const char *),
  *RegisterMagickInfo(MagickInfo *),
  *SetMagickInfo(const char *);

extern MagickExport MontageInfo
  *CloneMontageInfo(const ImageInfo *,const MontageInfo *);

extern MagickExport PixelPacket
  *(*GetImagePixels)
    (Image *,const int,const int,const unsigned int,const unsigned int),
  (*GetOnePixel)(Image *,const int,const int),
  *(*GetPixels)(const Image *),
  *(*SetImagePixels)
    (Image *,const int,const int,const unsigned int,const unsigned int);

extern MagickExport RectangleInfo
  GetImageBoundingBox(Image *);

extern MagickExport unsigned int
  AllocateImageColormap(Image *,const unsigned int),
  AnimateImages(const ImageInfo *image_info,Image *image),
  CompositeImage(Image *,const CompositeOperator,Image *,const int,const int),
  ContrastImage(Image *,const unsigned int),
  DispatchImage(Image *,const int,const int,const unsigned int,
    const unsigned int,const char *,const StorageType,void *),
  DisplayImages(const ImageInfo *image_info,Image *image),
  EqualizeImage(Image *),
  GammaImage(Image *,const char *),
  GetNumberScenes(const Image *),
  IsGeometry(const char *),
  IsGrayImage(Image *),
  IsMatteImage(Image *),
  IsMonochromeImage(Image *),
  IsPseudoClass(Image *),
  IsSubimage(const char *,const unsigned int),
  IsTainted(const Image *),
  LayerImage(Image *,const LayerType),
  ModulateImage(Image *,const char *),
  MogrifyImage(const ImageInfo *,const int,char **,Image **),
  MogrifyImages(const ImageInfo *,const int,char **,Image **),
  NegateImage(Image *,const unsigned int),
  NormalizeImage(Image *),
  OpaqueImage(Image *,const PixelPacket,const PixelPacket),
  PlasmaImage(Image *,const SegmentInfo *,int,int),
  PopImagePixels(const Image *,const QuantumType,unsigned char *),
  ProfileImage(Image *,const char *,const char *),
  PushImagePixels(const Image *,const QuantumType,const unsigned char *),
  QueryColorDatabase(const char *,PixelPacket *),
  RaiseImage(Image *,const RectangleInfo *,const int),
  ReadStream(const ImageInfo *,int (*)(const Image *,const void *,const size_t),
    ExceptionInfo *),
  RGBTransformImage(Image *,const ColorspaceType),
  SetImageInfo(ImageInfo *,const unsigned int),
  SignatureImage(Image *),
  SortColormapByIntensity(Image *),
  (*SyncImagePixels)(Image *),
  ThresholdImage(Image *,const double),
  TransformRGBImage(Image *,const ColorspaceType),
  TransmitImage(Image *,ImageInfo *,const TransmitType,void *,void *),
  TransparentImage(Image *,const PixelPacket),
  UnregisterMagickInfo(const char *),
  WriteImage(const ImageInfo *,Image *),
  WriteStream(ImageInfo *,Image *,
    int (*)(const Image *,const void *,const size_t));

extern MagickExport void
  AllocateNextImage(const ImageInfo *,Image *),
  (*CloseImagePixels)(Image *),
  CompressColormap(Image *),
  CycleColormapImage(Image *,const int),
  DescribeImage(Image *,FILE *,const unsigned int),
  DestroyImage(Image *),
  DestroyImageInfo(ImageInfo *),
  (*DestroyImagePixels)(Image *),
  DestroyImages(Image *),
  DestroyMontageInfo(MontageInfo *),
  GetImageInfo(ImageInfo *),
  GetMontageInfo(const ImageInfo *,MontageInfo *),
  GetPageInfo(RectangleInfo *),
  GetPixelPacket(PixelPacket *),
  ListMagickInfo(FILE *),
  MatteImage(Image *,Quantum),
  MagickIncarnate(const char *),
  Register8BIMImage(void),
  RegisterAVIImage(void),
  RegisterAVSImage(void),
  RegisterBIEImage(void),
  RegisterBMPImage(void),
  RegisterBMP24Image(void),
  RegisterCACHEImage(void),
  RegisterCMYKImage(void),
  RegisterDCMImage(void),
  RegisterDCXImage(void),
  RegisterDIBImage(void),
  RegisterDPSImage(void),
  RegisterEPDFImage(void),
  RegisterEPIImage(void),
  RegisterEPSImage(void),
  RegisterEPS2Image(void),
  RegisterEPSFImage(void),
  RegisterEPSIImage(void),
  RegisterEPTImage(void),
  RegisterFAXImage(void),
  RegisterFITSImage(void),
  RegisterFPXImage(void),
  RegisterG3Image(void),
  RegisterGIFImage(void),
  RegisterGIF87Image(void),
  RegisterGRADIENTImage(void),
  RegisterGRANITEImage(void),
  RegisterGRAYImage(void),
  RegisterHImage(void),
  RegisterHDFImage(void),
  RegisterHISTOGRAMImage(void),
  RegisterHTMImage(void),
  RegisterHTMLImage(void),
  RegisterICBImage(void),
  RegisterICMImage(void),
  RegisterICONImage(void),
  RegisterIMPLICITImage(void),
  RegisterIPTCImage(void),
  RegisterJBGImage(void),
  RegisterJBIGImage(void),
  RegisterJPGImage(void),
  RegisterJPEGImage(void),
  RegisterJPEG24Image(void),
  RegisterLABELImage(void),
  RegisterLOGOImage(void),
  RegisterMAPImage(void),
  RegisterMATTEImage(void),
  RegisterMIFFImage(void),
  RegisterMNGImage(void),
  RegisterMONOImage(void),
  RegisterMTVImage(void),
  RegisterMVGImage(void),
  RegisterNETSCAPEImage(void),
  RegisterNULLImage(void),
  RegisterP7Image(void),
  RegisterPBMImage(void),
  RegisterPCDImage(void),
  RegisterPCDSImage(void),
  RegisterPCLImage(void),
  RegisterPCTImage(void),
  RegisterPCXImage(void),
  RegisterPDBImage(void),
  RegisterPDFImage(void),
  RegisterPICImage(void),
  RegisterPICTImage(void),
  RegisterPICT24Image(void),
  RegisterPIXImage(void),
  RegisterPLASMAImage(void),
  RegisterPGMImage(void),
  RegisterPMImage(void),
  RegisterPNGImage(void),
  RegisterPNMImage(void),
  RegisterPPMImage(void),
  RegisterPREVIEWImage(void),
  RegisterPSImage(void),
  RegisterPS2Image(void),
  RegisterPS3Image(void),
  RegisterPSDImage(void),
  RegisterPTIFImage(void),
  RegisterPWPImage(void),
  RegisterRASImage(void),
  RegisterRGBImage(void),
  RegisterRGBAImage(void),
  RegisterRLAImage(void),
  RegisterRLEImage(void),
  RegisterSCTImage(void),
  RegisterSFWImage(void),
  RegisterSGIImage(void),
  RegisterSHTMLImage(void),
  RegisterSTEGANOImage(void),
  RegisterSUNImage(void),
  RegisterSVGImage(void),
  RegisterTEXTImage(void),
  RegisterTGAImage(void),
  RegisterTIFImage(void),
  RegisterTIFFImage(void),
  RegisterTIFF24Image(void),
  RegisterTILEImage(void),
  RegisterTIMImage(void),
  RegisterTTFImage(void),
  RegisterTXTImage(void),
  RegisterUILImage(void),
  RegisterUYVYImage(void),
  RegisterVDAImage(void),
  RegisterVICARImage(void),
  RegisterVIDImage(void),
  RegisterVIFFImage(void),
  RegisterVSTImage(void),
  RegisterWBMPImage(void),
  RegisterWPGImage(void),
  RegisterXImage(void),
  RegisterXBMImage(void),
  RegisterXCImage(void),
  RegisterXPMImage(void),
  RegisterXVImage(void),
  RegisterXWDImage(void),
  RegisterYUVImage(void),
  SetImage(Image *,Quantum),
  SolarizeImage(Image *,const double),
  SyncImage(Image *),
  TextureImage(Image *,Image *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  TransformImage(Image **,const char *,const char *),
  Unregister8BIMImage(void),
  UnregisterAVIImage(void),
  UnregisterAVSImage(void),
  UnregisterBIEImage(void),
  UnregisterBMPImage(void),
  UnregisterBMP24Image(void),
  UnregisterCACHEImage(void),
  UnregisterCMYKImage(void),
  UnregisterDCMImage(void),
  UnregisterDCXImage(void),
  UnregisterDIBImage(void),
  UnregisterDPSImage(void),
  UnregisterEPDFImage(void),
  UnregisterEPIImage(void),
  UnregisterEPSImage(void),
  UnregisterEPS2Image(void),
  UnregisterEPSFImage(void),
  UnregisterEPSIImage(void),
  UnregisterEPTImage(void),
  UnregisterFAXImage(void),
  UnregisterFITSImage(void),
  UnregisterFPXImage(void),
  UnregisterG3Image(void),
  UnregisterGIFImage(void),
  UnregisterGIF87Image(void),
  UnregisterGRADIENTImage(void),
  UnregisterGRANITEImage(void),
  UnregisterGRAYImage(void),
  UnregisterHImage(void),
  UnregisterHDFImage(void),
  UnregisterHISTOGRAMImage(void),
  UnregisterHTMImage(void),
  UnregisterHTMLImage(void),
  UnregisterICBImage(void),
  UnregisterICMImage(void),
  UnregisterICONImage(void),
  UnregisterIMPLICITImage(void),
  UnregisterIPTCImage(void),
  UnregisterJBGImage(void),
  UnregisterJBIGImage(void),
  UnregisterJPGImage(void),
  UnregisterJPEGImage(void),
  UnregisterJPEG24Image(void),
  UnregisterLABELImage(void),
  UnregisterLOGOImage(void),
  UnregisterMAPImage(void),
  UnregisterMATTEImage(void),
  UnregisterMIFFImage(void),
  UnregisterMNGImage(void),
  UnregisterMONOImage(void),
  UnregisterMTVImage(void),
  UnregisterMVGImage(void),
  UnregisterNETSCAPEImage(void),
  UnregisterNULLImage(void),
  UnregisterP7Image(void),
  UnregisterPBMImage(void),
  UnregisterPCDImage(void),
  UnregisterPCDSImage(void),
  UnregisterPCLImage(void),
  UnregisterPCTImage(void),
  UnregisterPCXImage(void),
  UnregisterPDBImage(void),
  UnregisterPDFImage(void),
  UnregisterPICImage(void),
  UnregisterPICTImage(void),
  UnregisterPICT24Image(void),
  UnregisterPIXImage(void),
  UnregisterPLASMAImage(void),
  UnregisterPGMImage(void),
  UnregisterPMImage(void),
  UnregisterPNGImage(void),
  UnregisterPNMImage(void),
  UnregisterPPMImage(void),
  UnregisterPREVIEWImage(void),
  UnregisterPSImage(void),
  UnregisterPS2Image(void),
  UnregisterPS3Image(void),
  UnregisterPSDImage(void),
  UnregisterPTIFImage(void),
  UnregisterPWPImage(void),
  UnregisterRASImage(void),
  UnregisterRGBImage(void),
  UnregisterRGBAImage(void),
  UnregisterRLAImage(void),
  UnregisterRLEImage(void),
  UnregisterSCTImage(void),
  UnregisterSFWImage(void),
  UnregisterSGIImage(void),
  UnregisterSHTMLImage(void),
  UnregisterSTEGANOImage(void),
  UnregisterSUNImage(void),
  UnregisterSVGImage(void),
  UnregisterTEXTImage(void),
  UnregisterTGAImage(void),
  UnregisterTIFImage(void),
  UnregisterTIFFImage(void),
  UnregisterTIFF24Image(void),
  UnregisterTILEImage(void),
  UnregisterTIMImage(void),
  UnregisterTTFImage(void),
  UnregisterTXTImage(void),
  UnregisterUILImage(void),
  UnregisterUYVYImage(void),
  UnregisterVDAImage(void),
  UnregisterVICARImage(void),
  UnregisterVIDImage(void),
  UnregisterVIFFImage(void),
  UnregisterVSTImage(void),
  UnregisterWBMPImage(void),
  UnregisterWPGImage(void),
  UnregisterXImage(void),
  UnregisterXBMImage(void),
  UnregisterXCImage(void),
  UnregisterXPMImage(void),
  UnregisterXVImage(void),
  UnregisterXWDImage(void),
  UnregisterYUVImage(void);

/*
  Language dependent methods.
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport void
  DestroyMagickInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
