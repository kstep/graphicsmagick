/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   N   N   GGGG                              %
%                            P   P  NN  N  G                                  %
%                            PPPP   N N N  G  GG                              %
%                            P      N  NN  G   G                              %
%                            P      N   N   GGG                               %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               November 1997                                 %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"
#if defined(HasPNG)
#include "png.h"
#include "zlib.h"

/*
  Optional declarations. Define or undefine them as you like.
*/
/* #define PNG_DEBUG  -- turning this on breaks VisualC compiling */ 

/*
  Features under construction.  Define these to work on them.
*/
#undef MNG_OBJECT_BUFFERS
#undef MNG_BASI_SUPPORTED
#define PNG_SORT_PALETTE

/*
  This is temporary until I set up malloc'ed object attributes array.
  Recompile with MNG_MAX_OBJECTS = 65536 to avoid this limit but
  waste more memory.
*/
#define MNG_MAX_OBJECTS 256

/*
  If this is not defined, spec is interpreted strictly.  If it is
  defined, an attempt will be made to recover from some errors,
  including
      o global PLTE too short
*/
#undef MNG_LOOSE

/*
  Don't try to define PNG_MNG_FEATURES_SUPPORTED here.  Make sure
  it's defined in libpng/pngconf.h, version 1.0.9 or later.  It won't work
  with earlier versions of libpng.  From libpng-1.0.3a to libpng-1.0.8,
  PNG_READ|WRITE_EMPTY_PLTE were used but those have been deprecated in
  libpng in favor of PNG_MNG_FEATURES_SUPPORTED, so we set them here.
  PNG_MNG_FEATURES_SUPPORTED is disabled by default in libpng-1.0.9 and
  will be enabled by default in libpng-1.2.0.
*/
#if (PNG_LIBPNG_VER == 10009)  /* work around libpng-1.0.9 bug */
#  undef PNG_READ_EMPTY_PLTE_SUPPORTED
#  undef PNG_WRITE_EMPTY_PLTE_SUPPORTED
#endif
#ifdef PNG_MNG_FEATURES_SUPPORTED
#  ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
#    define PNG_READ_EMPTY_PLTE_SUPPORTED
#  endif
#  ifndef PNG_WRITE_EMPTY_PLTE_SUPPORTED
#    define PNG_WRITE_EMPTY_PLTE_SUPPORTED
#  endif
#endif

/*
  Maximum valid unsigned long in PNG/MNG chunks is (2^31)-1
  This macro is only defined in libpng-1.0.3a and later.
*/
#ifndef PNG_MAX_UINT
#define PNG_MAX_UINT (png_uint_32) 0x7fffffffL
#endif

/*
  Constant strings for known chunk types.  If you need to add a chunk,
  add a string holding the name here.   To make the code more
  portable, we use ASCII numbers like this, not characters.
*/

static png_byte FARDATA mng_MHDR[5] = { 77,  72,  68,  82, '\0'};
static png_byte FARDATA mng_BACK[5] = { 66,  65,  67,  75, '\0'};
static png_byte FARDATA mng_BASI[5] = { 66,  65,  83,  73, '\0'};
static png_byte FARDATA mng_CLIP[5] = { 67,  76,  73,  80, '\0'};
static png_byte FARDATA mng_CLON[5] = { 67,  76,  79,  78, '\0'};
static png_byte FARDATA mng_DEFI[5] = { 68,  69,  70,  73, '\0'};
static png_byte FARDATA mng_DHDR[5] = { 68,  72,  68,  82, '\0'};
static png_byte FARDATA mng_DISC[5] = { 68,  73,  83,  67, '\0'};
static png_byte FARDATA mng_ENDL[5] = { 69,  78,  68,  76, '\0'};
static png_byte FARDATA mng_FRAM[5] = { 70,  82,  65,  77, '\0'};
static png_byte FARDATA mng_IEND[5] = { 73,  69,  78,  68, '\0'};
static png_byte FARDATA mng_IHDR[5] = { 73,  72,  68,  82, '\0'};
static png_byte FARDATA mng_JHDR[5] = { 74,  72,  68,  82, '\0'};
static png_byte FARDATA mng_LOOP[5] = { 76,  79,  79,  80, '\0'};
static png_byte FARDATA mng_MAGN[5] = { 77,  65,  71,  78, '\0'};
static png_byte FARDATA mng_MEND[5] = { 77,  69,  78,  68, '\0'};
static png_byte FARDATA mng_MOVE[5] = { 77,  79,  86,  69, '\0'};
static png_byte FARDATA mng_PAST[5] = { 80,  65,  83,  84, '\0'};
static png_byte FARDATA mng_PLTE[5] = { 80,  76,  84,  69, '\0'};
static png_byte FARDATA mng_SAVE[5] = { 83,  65,  86,  69, '\0'};
static png_byte FARDATA mng_SEEK[5] = { 83,  69,  69,  75, '\0'};
static png_byte FARDATA mng_SHOW[5] = { 83,  72,  79,  87, '\0'};
static png_byte FARDATA mng_TERM[5] = { 84,  69,  82,  77, '\0'};
static png_byte FARDATA mng_bKGD[5] = { 98,  75,  71,  68, '\0'};
static png_byte FARDATA mng_cHRM[5] = { 99,  72,  82,  77, '\0'};
static png_byte FARDATA mng_gAMA[5] = {103,  65,  77,  65, '\0'};
static png_byte FARDATA mng_pHYg[5] = {112,  72,  89, 103, '\0'};
static png_byte FARDATA mng_pHYs[5] = {112,  72,  89, 115, '\0'};
static png_byte FARDATA mng_sBIT[5] = {115,  66,  73,  84, '\0'};
static png_byte FARDATA mng_sRGB[5] = {115,  82,  71,  66, '\0'};
static png_byte FARDATA mng_tRNS[5] = {116,  82,  78,  83, '\0'};

/*
static png_byte FARDATA mng_IDAT[5] = { 73,  68,  65,  84, '\0'};
static png_byte FARDATA mng_JDAT[5] = { 74,  68,  65,  84, '\0'};
static png_byte FARDATA mng_JSEP[5] = { 74,  83,  69,  80, '\0'};
static png_byte FARDATA mng_hIST[5] = {104,  73,  83,  84, '\0'};
static png_byte FARDATA mng_iCCP[5] = {105,  67,  67,  80, '\0'};
static png_byte FARDATA mng_iTXt[5] = {105,  84,  88, 116, '\0'};
static png_byte FARDATA mng_oFFs[5] = {111,  70,  70, 115, '\0'};
static png_byte FARDATA mng_sPLT[5] = {115,  80,  76,  84, '\0'};
static png_byte FARDATA mng_tEXt[5] = {116,  69,  88, 116, '\0'};
static png_byte FARDATA mng_tIME[5] = {116,  73,  77,  69, '\0'};
static png_byte FARDATA mng_zTXt[5] = {122,  84,  88, 116, '\0'};
*/

typedef struct _MngBox
{
  long
    left,
    right,
    top,
    bottom;
} MngBox;

typedef struct _MngPair
{
  long
    a,
    b;
} MngPair;

#ifdef MNG_OBJECT_BUFFERS
typedef struct _MngBuffer
{

  unsigned long
    height,
    width;

  Image
    *image;

  png_color
    plte[256];

  int
    reference_count;

  unsigned char
    alpha_sample_depth,
    compression_method,
    color_type,
    concrete,
    filter_method,
    frozen,
    image_type,
    interlace_method,
    pixel_sample_depth,
    plte_length,
    sample_depth,
    viewable;
} MngBuffer;
#endif

typedef struct _MngInfo
{

#ifdef MNG_OBJECT_BUFFERS
  MngBuffer
    *ob[MNG_MAX_OBJECTS];
#endif

  Image *
    image;

#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
  png_byte
     read_buffer[8];

  int
     bytes_in_read_buffer,
     found_empty_plte,
     have_saved_bkgd_index,
     saved_bkgd_index;
#endif

  int
    new_number_colors;

  long
    x_off[MNG_MAX_OBJECTS],
    y_off[MNG_MAX_OBJECTS];

  MngBox
    object_clip[MNG_MAX_OBJECTS];

  unsigned char
    /* These flags could be combined into one byte */
    exists[MNG_MAX_OBJECTS],
    frozen[MNG_MAX_OBJECTS],
    visible[MNG_MAX_OBJECTS],
    viewable[MNG_MAX_OBJECTS];

  long
    loop_count[256],
    loop_iteration[256];

  off_t
    loop_jump[256];

  unsigned char
    loop_active[256];

  png_colorp
    global_plte;

  png_color_8p
    global_sbit;

  png_byte
    global_trns[256];

  float
    global_gamma;

  ChromaticityInfo
    global_chrm;

  RenderingIntent
    global_srgb_intent;

  unsigned long
    global_x_pixels_per_unit,
    global_y_pixels_per_unit;

  unsigned int
    global_phys_unit_type;

  unsigned int
    basi_warning,
    clon_warning,
    dhdr_warning,
    jhdr_warning,
    magn_warning,
    past_warning,
    phyg_warning,
    phys_warning,
    sbit_warning,
    show_warning;

#ifdef MNG_BASI_SUPPORTED
  unsigned long
    basi_width,
    basi_height;

  unsigned int
    basi_depth,
    basi_color_type,
    basi_compression_method,
    basi_filter_type,
    basi_interlace_method,
    basi_red,
    basi_green,
    basi_blue,
    basi_alpha,
    basi_viewable;
#endif

  png_uint_16
    magn_first,
    magn_last,
    magn_mb,
    magn_ml,
    magn_mr,
    magn_mt,
    magn_mx,
    magn_my,
    magn_methx,
    magn_methy;

} MngInfo;
#endif

/*
  Forward declarations.
*/
static unsigned int
  WritePNGImage(const ImageInfo *,Image *);

#ifdef PNG_SORT_PALETTE
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p r e s s C o l o r m a p T r a n s F i r s t                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CompressColormapTransFirst compresses an image colormap removing
%  any duplicate and unused color entries and putting the transparent colors
%  first.
%
%  The format of the CompressColormapTransFirst method is:
%
%      unsigned int CompressColormapTransFirst(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static unsigned int CompressColormapTransFirst(Image *image)
{
  int
    remap_needed,
    transparent_pixels,
    k,
    zero=0;

  long
    j,
    new_number_colors,
    number_colors,
    top_used,
    y;

  PixelPacket
    *colormap;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  IndexPacket
    *map,
    *opacity;

  unsigned char
    *marker,
    have_transparency;

  /*
    Determine if colormap can be compressed.
  */
  assert(image != (Image *) NULL);
  if (image->storage_class != PseudoClass || image->colors > 256 ||
      image->colors < 2)
    return(False);
  marker=(unsigned char *) AcquireMemory(image->colors);
  if (marker == (unsigned char *) NULL)
    ThrowBinaryException((ExceptionType) ResourceLimitWarning,
      "Unable to compress colormap", "Memory allocation failed");
  opacity=(IndexPacket *)
    AcquireMemory(image->colors*sizeof(IndexPacket));
  if (opacity == (IndexPacket *) NULL)
    {
      LiberateMemory((void **) &marker);
      ThrowBinaryException((ExceptionType) ResourceLimitWarning,
        "Unable to compress colormap", "Memory allocation failed")
    }
  /*
    Mark colors that are present.
  */
  number_colors=(long) image->colors;
  for (i=0; i < number_colors; i++)
  {
    marker[i]=False;
    opacity[i]=OpaqueOpacity;
  }
  transparent_pixels=0;
  top_used=0;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      marker[(int) indexes[x]]=True;
      opacity[(int) indexes[x]]=p->opacity;
      if (indexes[x] > top_used)
         top_used=indexes[x];
      if (p->opacity != OpaqueOpacity)
         transparent_pixels++;
      p++;
    }
  }
  if (image->matte)
  {
    /*
      Mark background color, topmost occurrence if more than one.
    */
    for (i=number_colors-1; i; i--)
    {
      if (ColorMatch(image->colormap[i],image->background_color,zero))
        {
          marker[i]=True;
          break;
        }
    }
  }
  /*
    Unmark duplicates.
  */
  for (i=0; i < number_colors-1; i++)
    if (marker[i])
      {
        for (j=i+1; j<number_colors; j++)
          if ((opacity[i] == opacity[j]) &&
              (ColorMatch(image->colormap[i],image->colormap[j],zero)))
            marker[j]=False;
       }
  /*
    Count colors that still remain.
  */
  have_transparency=False;
  new_number_colors=0;
  for (i=0; i < number_colors; i++)
    if (marker[i])
      {
        new_number_colors++;
        if (opacity[i] != OpaqueOpacity)
          have_transparency=True;
      }
  if ((!have_transparency || (marker[0] && (opacity[0] == TransparentOpacity)))
      && (new_number_colors == number_colors))
    {
      /*
        No duplicate or unused entries, and transparency-swap not needed
      */
      LiberateMemory((void **) &marker);
      LiberateMemory((void **) &opacity);
      return(True);
    }

  remap_needed=False;
  if (top_used > new_number_colors)
     remap_needed=True;

  /*
    Compress colormap.
  */

  colormap=(PixelPacket *) AcquireMemory(image->colors*sizeof(PixelPacket));
  if (colormap == (PixelPacket *) NULL)
    {
      LiberateMemory((void **) &marker);
      LiberateMemory((void **) &opacity);
      ThrowBinaryException((ExceptionType) ResourceLimitWarning,
        "Unable to compress colormap", "Memory allocation failed")
    }
  /*
    Eliminate unused colormap entries.
  */
  map=(IndexPacket *) AcquireMemory(number_colors*sizeof(IndexPacket));
  if (map == (IndexPacket *) NULL)
    {
      LiberateMemory((void **) &marker);
      LiberateMemory((void **) &opacity);
      LiberateMemory((void **) &colormap);
      ThrowBinaryException((ExceptionType) ResourceLimitWarning,
        "Unable to compress colormap", "Memory allocation failed")
    }
  k=0;
  for (i=0; i < number_colors; i++)
  {
    map[i]=(IndexPacket) k;
    if (marker[i])
      {
        for (j=i+1; j < number_colors; j++)
        {
          if ((opacity[i] == opacity[j]) &&
              (ColorMatch(image->colormap[i],image->colormap[j],zero)))
            {
               map[j]=(IndexPacket) k;
               marker[j]=False;
            }
        }
        k++;
      }
  }
  j=0;
  for (i=0; i < number_colors; i++)
  {
    if (marker[i])
      {
        colormap[j]=image->colormap[i];
        j++;
      }
  }
  if (have_transparency && (opacity[0] != TransparentOpacity))
    {
      /*
        Move the first transparent color to palette entry 0.
      */
      for (i=1; i < number_colors; i++)
      {
        if (marker[i] && opacity[i] == TransparentOpacity)
          {
            PixelPacket
              temp_colormap;

            temp_colormap=colormap[0];
            colormap[0]=colormap[(int) map[i]];
            colormap[map[i]]=temp_colormap;
            for (j=0; j < number_colors; j++)
            {
              if (map[j] == 0)
                map[j]=map[i];
              else if (map[j] == map[i])
                map[j]=0;
            }
            remap_needed=True;
            break;
          }
      }
   }

  LiberateMemory((void **) &opacity);
  LiberateMemory((void **) &marker);

  if (remap_needed)
    {
      /*
      Remap pixels.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          j=(int) indexes[x];
          indexes[x]= map[j];
        }
        if (!SyncImagePixels(image))
          break;
      }
      for (i=0; i<new_number_colors; i++)
         image->colormap[i]=colormap[i];
    }
  LiberateMemory((void **) &colormap);
  image->colors=new_number_colors;
  LiberateMemory((void **) &map);
  return(True);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e I s G r a y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%   Like IsGrayImage except does not change DirectClass to PseudoClass        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
static unsigned int ImageIsGray(Image *image)
{
  register const PixelPacket
    *p;

  register long
    i,
    x,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (image->storage_class == PseudoClass)
    {
      for (i=0; i < (long) image->colors; i++)
        if (!IsGray(image->colormap[i]))
          return(False);
      return(True);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; (x < (long) image->columns); x++)
    {
       if (!IsGray(*p))
          return(False);
       p++;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e I s M o n o c h r o m e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%   Like IsMonochromeImage except does not change DirectClass to PseudoClass  %
%   and is more accurate.                                                     %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
static unsigned int ImageIsMonochrome(Image *image)
{
  register const PixelPacket
    *p;

  register long
    i,
    x,
    y,
    zero=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (image->storage_class == PseudoClass)
    {
      for (i=0; i < (long) image->colors; i++)
      {
        if (!IsGray(image->colormap[i]) || ((image->colormap[i].red != zero)
            && (image->colormap[i].red != MaxRGB)))
          return(False);
      }
      return(True);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      if ((p->red != 0) && (p->red != MaxRGB))
        return(False);
      if (!IsGray(*p))
        return(False);
      p++;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M N G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMNG returns True if the image format type, identified by the
%  magick string, is MNG.
%
%  The format of the IsMNG method is:
%
%      unsigned int IsMNG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMNG returns True if the image format type is MNG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsMNG(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(False);
  if (memcmp(magick,"\212MNG\r\n\032\n",8) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P N G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPNG returns True if the image format type, identified by the
%  magick string, is PNG.
%
%  The format of the IsPNG method is:
%
%      unsigned int IsPNG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPNG returns True if the image format type is PNG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPNG(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(False);
  if (memcmp(magick,"\211PNG\r\n\032\n",8) == 0)
    return(True);
  return(False);
}

#if defined(HasPNG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P N G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPNGImage reads a Portable Network Graphics (PNG) or
%  Multiple-image Network Graphics (MNG) image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image or set of images.
%
%  MNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu.
%
%  The format of the ReadPNGImage method is:
%
%      Image *ReadPNGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPNGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%  To do, more or less in chronological order (as of version 5.2.8,
%   January 23, 2001 -- glennrp -- see also "To do" under WritePNGImage):
%
%    Get 16-bit cheap transparency working.
%
%    (At this point, PNG decoding is supposed to be in full MNG-LC compliance)
%
%    Preserve all unknown and not-yet-handled known chunks found in input
%    PNG file and copy them into output PNG files according to the PNG
%    copying rules.
%
%    (At this point, PNG encoding should be in full MNG compliance)
%
%    Provide options for choice of background to use when the MNG BACK
%    chunk is not present or is not mandatory (i.e., leave transparent,
%    user specified, MNG BACK, PNG bKGD)
%
%    Implement LOOP/ENDL [done, but could do discretionary loops more
%    efficiently by linking in the duplicate frames.].
%
%    Decode JNG datastreams.
%
%    Decode and act on the MHDR simplicity profile (offer option to reject
%    files or attempt to process them anyway when the profile isn't LC or VLC).
%
%    Upgrade to full MNG without Delta-PNG.
%
%        o  BACK [done a while ago except for background image ID]
%        o  MOVE [done 15 May 1999]
%        o  CLIP [done 15 May 1999]
%        o  DISC [done 19 May 1999]
%        o  SAVE [partially done 19 May 1999 (marks objects frozen)]
%        o  SEEK [partially done 19 May 1999 (discard function only)]
%        o  SHOW
%        o  PAST
%        o  BASI
%        o  MNG-level tEXt/zTXt
%        o  pHYg
%        o  pHYs
%        o  sBIT
%        o  bKGD
%        o  iTXt (wait for libpng implementation).
%
%    Use the scene signature to discover when an identical scene is
%    being reused, and just point to the original image->pixels instead
%    of storing another set of pixels.  This is not specific to MNG
%    but could be applied generally.
%
%    Upgrade to full MNG with Delta-PNG.
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(PNG_NO_STDIO)
/*
  This is the function that does the actual reading of data.  It is
  the same as the one supplied in libpng, except that it receives the
  datastream from the ReadBlob() function instead of standard input.
*/
static void png_get_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  Image
    *image;

  image=(Image *) png_get_io_ptr(png_ptr);
  if (length)
    {
      png_size_t
        check;

      check=(png_size_t) ReadBlob(image,(size_t) length,(char *) data);
      if (check != length)
        png_error(png_ptr,"Read Exception");
    }
}

#if !defined(PNG_READ_EMPTY_PLTE_SUPPORTED) && \
    !defined(PNG_MNG_FEATURES_SUPPORTED)
/* We use mng_get_data() instead of png_get_data() if we have a libpng
 * older than libpng-1.0.3a, which was the first to allow the empty
 * PLTE, or a newer libpng in which PNG_MNG_FEATURES_SUPPORTED was
 * ifdef'ed out.  Earlier versions would crash if the bKGD chunk was
 * encountered after an empty PLTE, so we have to look ahead for bKGD
 * chunks and remove them from the datastream that is passed to libpng,
 * and store their contents for later use.
 */
static void mng_get_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  MngInfo
    *mng_info;

  Image
    *image;

  png_size_t
    check;

  register int
    i;

  i=0;
  mng_info=(MngInfo *) png_get_io_ptr(png_ptr);
  image=(Image *) mng_info->image;
  while (mng_info->bytes_in_read_buffer && length)
  {
    data[i]=mng_info->read_buffer[i];
    mng_info->bytes_in_read_buffer--;
    length--;
    i++;
  }
  if (length)
    {
      check=(png_size_t) ReadBlob(image,(size_t) length,(char *) data);
      if (check != length)
        png_error(png_ptr,"Read Exception");
      if (length == 4)
        {
          if ((data[0] == 0) && (data[1] == 0) && (data[2] == 0) &&
              (data[3] == 0))
            {
              check=(png_size_t) ReadBlob(image,(size_t) length,
                (char *) mng_info->read_buffer);
              mng_info->read_buffer[4]=0;
              mng_info->bytes_in_read_buffer=4;
              if (!memcmp(mng_info->read_buffer,mng_PLTE,4))
                mng_info->found_empty_plte=True;
              if (!memcmp(mng_info->read_buffer,mng_IEND,4))
                {
                  mng_info->found_empty_plte=False;
                  mng_info->have_saved_bkgd_index=False;
                }
            }
          if ((data[0] == 0) && (data[1] == 0) && (data[2] == 0) &&
              (data[3] == 1))
            {
              check=(png_size_t) ReadBlob(image,(size_t) length,
                (char *) mng_info->read_buffer);
              mng_info->read_buffer[4]=0;
              mng_info->bytes_in_read_buffer=4;
              if (!memcmp(mng_info->read_buffer,mng_bKGD,4))
                if (mng_info->found_empty_plte)
                  {
                    /*
                      Skip the bKGD data byte and CRC.
                    */
                    check=(png_size_t)
                      ReadBlob(image,5,(char *) mng_info->read_buffer);
                    check=(png_size_t) ReadBlob(image,(size_t) length,
                      (char *) mng_info->read_buffer);
                    mng_info->saved_bkgd_index=mng_info->read_buffer[0];
                    mng_info->have_saved_bkgd_index=True;
                    mng_info->bytes_in_read_buffer=0;
                  }
            }
        }
    }
}
#endif

static void png_put_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  Image
    *image;

  image=(Image *) png_get_io_ptr(png_ptr);
  if (length)
    {
      png_size_t
        check;

      check=(png_size_t) WriteBlob(image,(unsigned long) length,(char *) data);
      if (check != length)
        png_error(png_ptr,"Write Exception");
    }
}

static void png_flush_data(png_structp png_ptr)
{
  Image
    *image;

  image=(Image *) png_get_io_ptr(png_ptr);
  (void) SyncBlob(image);
}
#endif

#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
static int PalettesAreEqual(Image *a,Image *b)
{
  long
    i;

  if ((a == (Image *) NULL) || (b == (Image *) NULL))
    return((int) False);
  if (a->storage_class!=PseudoClass || b->storage_class!=PseudoClass)
    return((int) False);
  if (a->colors != b->colors)
    return((int) False);
  for (i=0; i < (long) a->colors; i++)
  {
    if ((a->colormap[i].red != b->colormap[i].red) ||
        (a->colormap[i].green != b->colormap[i].green) ||
        (a->colormap[i].blue != b->colormap[i].blue))
      return((int) False);
  }
  return((int) True);
}
#endif

static void MngInfoDiscardObject(MngInfo *mng_info,int i)
{
  if (i && (i < MNG_MAX_OBJECTS) && (mng_info != (MngInfo *) NULL) &&
      mng_info->exists[i] && !mng_info->frozen[i])
    {
#ifdef MNG_OBJECT_BUFFERS
      if (mng_info->ob[i] != (MngBuffer *) NULL)
        {
          if (mng_info->ob[i]->reference_count > 0)
            mng_info->ob[i]->reference_count--;
          if (mng_info->ob[i]->reference_count == 0)
            {
              if (mng_info->ob[i]->image != (Image *) NULL)
                DestroyImage(mng_info->ob[i]->image);
              LiberateMemory((void **) &mng_info->ob[i]);
            }
        }
      mng_info->ob[i]=(MngBuffer *) NULL;
#endif
      mng_info->exists[i]=False;
      mng_info->visible[i]=True;
      mng_info->viewable[i]=False;
      mng_info->frozen[i]=False;
      mng_info->x_off[i]=0;
      mng_info->y_off[i]=0;
      mng_info->object_clip[i].left=0;
      mng_info->object_clip[i].right=PNG_MAX_UINT;
      mng_info->object_clip[i].top=0;
      mng_info->object_clip[i].bottom=PNG_MAX_UINT;
    }
}

static void MngInfoFreeStruct(MngInfo *mng_info,int *have_mng_structure)
{
  if (*have_mng_structure && (mng_info != (MngInfo *) NULL))
    {
      register int
        i;

      for (i=1; i < MNG_MAX_OBJECTS; i++)
        MngInfoDiscardObject(mng_info,i);
      if (mng_info->global_plte != (png_colorp) NULL)
        LiberateMemory((void **) &mng_info->global_plte);
      if (mng_info->global_sbit != (png_color_8p) NULL)
        LiberateMemory((void **) &mng_info->global_sbit);
      LiberateMemory((void **) &mng_info);
      *have_mng_structure=False;
    }
}

static MngBox mng_minimum_box(MngBox box1,MngBox box2)
{
  MngBox
    box;

  box=box1;
  if (box.left < box2.left)
    box.left=box2.left;
  if (box.top < box2.top)
    box.top=box2.top;
  if (box.right > box2.right)
    box.right=box2.right;
  if (box.bottom > box2.bottom)
    box.bottom=box2.bottom;
  return box;
}

static MngBox mng_read_box(MngBox previous_box,char delta_type,unsigned char *p)
{
   MngBox
      box;

  /*
    Read clipping boundaries from DEFI, CLIP, FRAM, or PAST chunk.
  */
  box.left=(long) ((p[0]  << 24) | (p[1]  << 16) | (p[2]  << 8) | p[3]);
  box.right=(long) ((p[4]  << 24) | (p[5]  << 16) | (p[6]  << 8) | p[7]);
  box.top=(long) ((p[8]  << 24) | (p[9]  << 16) | (p[10] << 8) | p[11]);
  box.bottom=(long) ((p[12] << 24) | (p[13] << 16) | (p[14] << 8) | p[15]);
  if (delta_type != 0)
    {
      box.left+=previous_box.left;
      box.right+=previous_box.right;
      box.top+=previous_box.top;
      box.bottom+=previous_box.bottom;
    }
  return(box);
}

static MngPair mng_read_pair(MngPair previous_pair,char delta_type,
  unsigned char *p)
{
  MngPair
    pair;

  /*
    Read two longs from MHDR, CLON, MOVE or PAST chunk
  */
  pair.a=(long) ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
  pair.b=(long) ((p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7]);
  if (delta_type != 0)
    {
      pair.a+=previous_pair.a;
      pair.b+=previous_pair.b;
    }
  return(pair);
}

static long mng_get_long(unsigned char *p)
{
  return((long) ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]));
}

static void PNGErrorHandler(png_struct *ping,png_const_charp message)
{
  Image
    *image;

  image=(Image *) png_get_error_ptr(ping);
#ifdef PNG_DEBUG
  (void) printf("libpng-%.1024s error: %.1024s\n", PNG_LIBPNG_VER_STRING, message);
#endif
  ThrowException(&image->exception,(ExceptionType) DelegateWarning,message,
     image->filename);
  longjmp(ping->jmpbuf,1);
}

static void PNGWarningHandler(png_struct *ping,png_const_charp message)
{
  Image
    *image;

#ifdef PNG_DEBUG
  (void) printf("libpng-%.1024s warning: %.1024s\n", PNG_LIBPNG_VER_STRING, message);
#endif
  image=(Image *) png_get_error_ptr(ping);
  ThrowException(&image->exception,(ExceptionType) DelegateWarning,message,
     image->filename);
}

#ifdef PNG_USER_MEM_SUPPORTED
static png_voidp png_IM_malloc(png_structp png_ptr,png_uint_32 size)
{
#if (PNG_LIBPNG_VER < 10011)
  png_voidp
    ret;

  png_ptr=png_ptr;
  ret = ((png_voidp) AcquireMemory((size_t) size));
  if (ret == NULL)
    png_error("Insufficient memory.");
  return (ret);
#else
  png_ptr=png_ptr;
  return((png_voidp) AcquireMemory((size_t) size));
#endif
}

/*
  Free a pointer.  It is removed from the list at the same time.
*/
static png_free_ptr png_IM_free(png_structp png_ptr,png_voidp ptr)
{
  png_ptr=png_ptr;
  LiberateMemory((void **) &ptr);
  return((png_free_ptr) NULL);
}
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static int
png_read_raw_profile(Image *image, const ImageInfo *image_info,
   png_textp text,int ii)
{
   unsigned char
     *info;

   register long
     i;

   register unsigned char
     *dp;

   register png_charp
     sp;

   png_uint_32
     length,
     nibbles;

   unsigned char
     unhex[103]= {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,1, 2,3,4,5,6,7,8,9,0,0,
                  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,10,11,12,
                  13,14,15};

   sp=text[ii].text+1;
   /* look for newline */
   while (*sp != '\n')
      sp++;
   /* look for length */
   while (*sp == '\0' || *sp == ' ' || *sp == '\n')
      sp++;
   length=atol(sp);
   while (*sp != ' ' && *sp != '\n')
      sp++;
   /* allocate space */
   info=(unsigned char *) AcquireMemory(length);
   if (info == (unsigned char *) NULL)
     ThrowBinaryException((ExceptionType) ResourceLimitWarning,
        "Unable to copy profile", "Memory allocation failed");
   /* copy profile, skipping white space and column 1 "=" signs */
   dp=info;
   nibbles=length*2;
   for (i=0; i < (long) nibbles; i++)
   {
     while (*sp < '0' || (*sp > '9' && *sp < 'a') || *sp > 'f')
        sp++;
     if (i%2 == 0)
        *dp=16*unhex[(int) *sp++];
     else
        (*dp++)+=unhex[(int) *sp++];
   }

   /* We have already read "Raw profile type " */
   if (!memcmp(&text[ii].key[17], "iptc\0",5))
     {
       image->iptc_profile.length=length;
       image->iptc_profile.info=info;
       if (image_info->verbose)
         (void) printf(" Found an IPTC profile.\n");
     }
   else if (!memcmp(&text[ii].key[17], "icm\0",4))
     {
       image->color_profile.length=length;
       image->color_profile.info=info;
       if (image_info->verbose)
         (void) printf(" Found an ICM (ICCP) profile.\n");
     }
   else
     {
       i=(long) image->generic_profiles;
       if (image->generic_profile == (ProfileInfo *) NULL)
         image->generic_profile=(ProfileInfo *)
            AcquireMemory(sizeof(ProfileInfo));
       else
         ReacquireMemory((void **) &image->generic_profile,
            (i+1)*sizeof(ProfileInfo));
       image->generic_profile[i].length=length;
       image->generic_profile[i].name=AllocateString(&text[ii].key[17]);
       image->generic_profile[i].info=info;
       image->generic_profiles++;
       if (image_info->verbose)
         (void) printf(" Found a generic profile, type %.1024s\n", &text[ii].key[17]);
     }
   return True;
}

static Image *ReadPNGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    page_geometry[MaxTextExtent];

  Image
    *image;

  int
    have_global_bkgd,
    have_global_chrm,
    have_global_gama,
    have_global_phys,
    have_global_sbit,
    have_global_srgb,
    first_mng_object,
    have_mng_structure,
    num_text,
    object_id,
    term_chunk_found,
    skip_to_iend;

  long
    image_count = 0,
    image_found,
    y;

  MngInfo
    *mng_info;

  MngBox
    clip,
    default_fb,
    fb,
    frame,
    image_box,
    previous_fb;

  PixelPacket
    mng_background_color,
    mng_global_bkgd,
    transparent_color;

  register unsigned char
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  register long
    i;

  register PixelPacket
    *q;

  png_info
    *end_info,
    *ping_info;

  png_struct
    *ping;

  size_t
    count;

  short
    loop_level,
    loops_active,
    skipping_loop;

  unsigned char
    *png_pixels,
    **scanlines;

  unsigned int
    framing_mode = 1,
    mandatory_back = 0,
#ifdef MNG_OBJECT_BUFFERS
    mng_background_object = 0,
#endif
    mng_type = 0,   /* 0: PNG; 1: MNG; 2: MNG-LC; 3: MNG-VLC */
    status;

  unsigned long
    default_frame_delay,
    default_frame_timeout,
    final_delay,
    frame_delay,
    frame_timeout,
    global_plte_length,
    global_trns_length,
    image_height,
    image_width,
    insert_layers,
    length,
    mng_height = 0,
    mng_iterations=0,
    mng_width = 0,
    simplicity = 0,
    subframe_height=0,
    subframe_width=0,
    ticks_per_second;

    png_textp
      text;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  mng_info=(MngInfo *) NULL;
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException((ExceptionType) FileOpenWarning,
    "Unable to open file",image);
  first_mng_object=False;
  skipping_loop=(-1);
  have_mng_structure=False;
  if (LocaleCompare(image_info->magick,"MNG") == 0)
    {
      char
        magic_number[MaxTextExtent];

      /*
        Verify MNG signature.
      */
      (void) ReadBlob(image,8,magic_number);
      if (memcmp(magic_number,"\212MNG\r\n\032\n",8) != 0)
        ThrowReaderException((ExceptionType) CorruptImageWarning,
        "Not a MNG image file",image);
      first_mng_object=True;
      /*
        Allocate a MngInfo structure.
      */
      mng_info=(MngInfo *) AcquireMemory(sizeof(MngInfo));
      if (mng_info == (MngInfo *) NULL)
        ThrowReaderException((ExceptionType) ResourceLimitWarning,
        "Memory allocation failed",
          image);
      have_mng_structure=True;
      mng_info->image=image;
      mng_info->global_plte=(png_colorp) NULL;
      mng_info->global_sbit=(png_color_8p) NULL;
      mng_info->basi_warning=False;
      mng_info->clon_warning=False;
      mng_info->dhdr_warning=False;
      mng_info->jhdr_warning=False;
      mng_info->magn_warning=False;
      mng_info->past_warning=False;
      mng_info->phyg_warning=False;
      mng_info->phys_warning=False;
      mng_info->sbit_warning=False;
      mng_info->show_warning=False;
      mng_info->magn_methx=0;
      mng_info->magn_methy=0;

      for (i=0; i < MNG_MAX_OBJECTS; i++)
      {
        mng_info->exists[i]=False;
        mng_info->visible[i]=True;
        mng_info->viewable[i]=False;
        mng_info->frozen[i]=False;
        mng_info->x_off[i]=0;
        mng_info->y_off[i]=0;
        mng_info->object_clip[i].left=0;
        mng_info->object_clip[i].right=PNG_MAX_UINT;
        mng_info->object_clip[i].top=0;
        mng_info->object_clip[i].bottom=PNG_MAX_UINT;
#ifdef MNG_OBJECT_BUFFERS
        mng_info->ob[i]=(MngBuffer *) NULL;
#endif
      }
      mng_info->exists[0]=True;
      loop_level=0;
      loops_active=0;
      for (i=0; i < 256; i++)
      {
        mng_info->loop_active[i]=0;
        mng_info->loop_count[i]=0;
        mng_info->loop_iteration[i]=0;
        mng_info->loop_jump[i]=0;
      }
    }
  mng_type=0;
  insert_layers=False;  /* should be False when converting or mogrifying */
  default_frame_delay=0;
  default_frame_timeout=0;
  frame_delay=0;
  final_delay=1;
  ticks_per_second=100;
  object_id=0;
  skip_to_iend=False;
  term_chunk_found=False;
  image_found=False;
  framing_mode=1;
  mandatory_back=False;
  have_global_chrm=False;
  have_global_bkgd=False;
  have_global_gama=False;
  have_global_phys=False;
  have_global_sbit=False;
  have_global_srgb=False;
  global_plte_length=0;
  global_trns_length=0;
  mng_background_color=image->background_color;
  do
  {
    if (LocaleCompare(image_info->magick,"MNG") == 0)
      {
        char
          type[MaxTextExtent];

        unsigned char
          *chunk;

        /*
          Read a new chunk.
        */
        type[0]='\0';
        (void) strcat(type,"errr");
        length=ReadBlobMSBLong(image);
        count=ReadBlob(image,4,type);
        if (length > PNG_MAX_UINT)
          status=False;
        if (count == 0)
          ThrowReaderException((ExceptionType) CorruptImageWarning,
          "Corrupt MNG image",image);
        if (!memcmp(type,mng_JHDR,4))
          {
            skip_to_iend=True;
            if (!mng_info->jhdr_warning)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "JNG is not implemented yet",image->filename);
            mng_info->jhdr_warning++;
          }
        if (!memcmp(type,mng_DHDR,4))
          {
            skip_to_iend=True;
            if (!mng_info->dhdr_warning)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "Delta-PNG is not implemented yet",image->filename);
            mng_info->dhdr_warning++;
          }
       p=NULL;
       if (length)
          {
            chunk=(unsigned char *) AcquireMemory(length);
            if (chunk == (unsigned char *) NULL)
              ThrowReaderException((ExceptionType) ResourceLimitWarning,
               "Unable to allocate memory for chunk data",image);
            for (i=0; i < (long) length; i++)
              chunk[i]=ReadBlobByte(image);
            p=chunk;
          }
        (void) ReadBlobMSBLong(image);  /* read crc word */
        if (!memcmp(type,mng_MEND,4))
          break;
        if (skip_to_iend)
          {
            if (!memcmp(type,mng_IEND,4))
              skip_to_iend=False;
            if (length)
              LiberateMemory((void **) &chunk);
            continue;
          }

        if (!memcmp(type,mng_MHDR,4))
          {
            MngPair
               pair;

            pair.a=0;
            pair.b=0;
            pair=mng_read_pair(pair,0,p);
            mng_width=(unsigned long) pair.a;
            mng_height=(unsigned long) pair.b;
            p+=8;
            ticks_per_second=mng_get_long(p);
            if (ticks_per_second == 0)
              default_frame_delay=0;
            else
              default_frame_delay=100/ticks_per_second;
            frame_delay=default_frame_delay;
            simplicity=0;
            if (length > 16)
              {
                p+=16;
                simplicity=mng_get_long(p);
              }
            mng_type=1;    /* Full MNG */
            if ((simplicity != 0) && ((simplicity | 11) == 11))
              mng_type=2; /* LC */
            if ((simplicity != 0) && ((simplicity | 9) == 9))
              mng_type=3; /* VLC */
            if (mng_type != 3)
              insert_layers=True;
            if (GetPixels(image) != (PixelPacket *) NULL)
              {
                /*
                  Allocate next image structure.
                */
                AllocateNextImage(image_info,image);
                if (image->next == (Image *) NULL)
                  {
                    DestroyImages(image);
                    MngInfoFreeStruct(mng_info,&have_mng_structure);
                    return((Image *) NULL);
                  }
                *image->next->blob=(*image->blob);
                image=image->next;
                mng_info->image=image;
              }

            if ((mng_width > 65535) || (mng_height > 65535))
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "image dimensions are too large.",image->filename);
            FormatString(page_geometry,"%lux%lu%+0+0",mng_width,mng_height);
            frame.left=0;
            frame.right=(long) mng_width;
            frame.top=0;
            frame.bottom=(long) mng_height;
            clip=default_fb=previous_fb=frame;
            for (i=0; i < MNG_MAX_OBJECTS; i++)
              mng_info->object_clip[i]=frame;
            LiberateMemory((void **) &chunk);
            continue;
          }

        if (!memcmp(type,mng_TERM,4))
          {
            int
              repeat=0;


            if (length)
              repeat=p[0];
            if (repeat == 3)
              {
                final_delay=(png_uint_32) mng_get_long(&p[2]);
                mng_iterations=(png_uint_32) mng_get_long(&p[6]);
                if (mng_iterations == PNG_MAX_UINT)
                  mng_iterations=0;
                image->iterations=mng_iterations;
                term_chunk_found=True;
              }
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_DEFI,4))
          {
            if (mng_type == 3)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "DEFI chunk found in MNG-VLC datastream",(char *) NULL);
            object_id = (p[0]<<8) | p[1];
            if (mng_type == 2 && object_id != 0)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "Nonzero object_id in MNG-LC datastream",(char *) NULL);
            if (object_id > MNG_MAX_OBJECTS)
              {
                /*
                  Instead of issuing a warning we should allocate a larger
                  MngInfo structure and continue.
                */
                ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                  "object id too large",(char *) NULL);
                object_id=MNG_MAX_OBJECTS;
              }
            if (mng_info->exists[object_id])
              if (mng_info->frozen[object_id])
                {
                  LiberateMemory((void **) &chunk);
                  ThrowException(&image->exception,(ExceptionType)
                     DelegateWarning,
                     "DEFI cannot redefine a frozen MNG object",(char *) NULL);
                  continue;
                }
            mng_info->exists[object_id]=True;
            if (length > 2)
              mng_info->visible[object_id]=!p[2];
            /*
              Extract object offset info.
            */
            if (length>11)
              {
                MngPair
                  pair;

                pair.a=0;
                pair.b=0;
                pair=mng_read_pair(pair,0,&p[4]);
                mng_info->x_off[object_id]=pair.a;
                mng_info->y_off[object_id]=pair.b;
              }
            /*
              Extract object clipping info.
            */
            if (length>27)
              mng_info->object_clip[object_id]=mng_read_box(frame,0,&p[12]);
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_bKGD,4))
          {
            have_global_bkgd=False;
            if (length > 5)
              {
                mng_global_bkgd.red=
                  (unsigned short) XDownScale((int) ((p[0]<<8) | p[1]));
                mng_global_bkgd.green =
                  (unsigned short) XDownScale((int) ((p[2]<<8) | p[3]));
                mng_global_bkgd.blue  =
                  (unsigned short) XDownScale((int) ((p[4]<<8) | p[5]));
                have_global_bkgd=True;
              }
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_BACK,4))
          {
            if (length>6)
              mandatory_back=p[6];
            else
              mandatory_back=0;
            if (mandatory_back && length>5)
              {
                mng_background_color.red=
                  (unsigned short) XDownScale((int) ((p[0]<<8) | p[1]));
                mng_background_color.green=
                  (unsigned short) XDownScale((int) ((p[2]<<8) | p[3]));
                mng_background_color.blue=
                  (unsigned short) XDownScale((int) ((p[4]<<8) | p[5]));
                mng_background_color.opacity=OpaqueOpacity;
              }
#ifdef MNG_OBJECT_BUFFERS
            if (length > 8)
              mng_background_object=(p[7] << 8) | p[8];
#endif
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_PLTE,4))
          {
            register int
             i;

            /*
              Read global PLTE.
            */
            if (length && (length < 769))
              {
                if (mng_info->global_plte == (png_colorp) NULL)
                  mng_info->global_plte=
                   (png_colorp) AcquireMemory(256*sizeof(png_color));
                for (i=0; i< (long) (length/3); i++)
                {
                  mng_info->global_plte[i].red=p[3*i];
                  mng_info->global_plte[i].green=p[3*i+1];
                  mng_info->global_plte[i].blue=p[3*i+2];
                }
                global_plte_length=length/3;
              }
#ifdef MNG_LOOSE
            for ( ; i < 256; i++)
            {
              mng_info->global_plte[i].red=i;
              mng_info->global_plte[i].green=i;
              mng_info->global_plte[i].blue=i;
            }
            if (length)
              global_plte_length=256;
#endif
            else
              global_plte_length=0;
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_tRNS,4))
          {
            register int
              i;

            /* read global tRNS */

            if (length < 257)
              for (i=0; i<(long) length; i++)
                mng_info->global_trns[i]=p[i];

#ifdef MNG_LOOSE
            for ( ; i<256; i++)
              mng_info->global_trns[i]=255;
#endif
            global_trns_length=length;
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_gAMA,4))
          {
            if (length == 4)
              {
                long igamma=mng_get_long(p);
                mng_info->global_gamma=((float) igamma)*0.00001;
                have_global_gama=True;
              }
            else
              have_global_gama=False;
            LiberateMemory((void **) &chunk);
            continue;
          }

        if (!memcmp(type,mng_cHRM,4))
          {
            /*
              Read global cHRM
            */
            if (length == 32)
              {
                mng_info->global_chrm.white_point.x=0.00001*mng_get_long(p);
                mng_info->global_chrm.white_point.y=0.00001*mng_get_long(&p[4]);
                mng_info->global_chrm.red_primary.x=0.00001*mng_get_long(&p[8]);
                mng_info->global_chrm.red_primary.y=0.00001*mng_get_long(&p[12]);
                mng_info->global_chrm.green_primary.x=0.00001*mng_get_long(&p[16]);
                mng_info->global_chrm.green_primary.y=0.00001*mng_get_long(&p[20]);
                mng_info->global_chrm.blue_primary.x=0.00001*mng_get_long(&p[24]);
                mng_info->global_chrm.blue_primary.y=0.00001*mng_get_long(&p[28]);
                have_global_chrm=True;
              }
            else
              have_global_chrm=False;
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_sRGB,4))
          {
            /*
              Read global sRGB.
            */
            if (length)
              {
                mng_info->global_srgb_intent=(RenderingIntent) p[0];
                have_global_srgb=True;
              }
            else
              have_global_srgb=False;
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_FRAM,4))
          {
            if (mng_type == 3)
              ThrowException(&image->exception,(ExceptionType)
                DelegateWarning,
                "FRAM chunk found in MNG-VLC datastream",(char *) NULL);
            if ((framing_mode == 2) || (framing_mode == 4))
              image->delay=frame_delay;
            frame_delay=default_frame_delay;
            frame_timeout=default_frame_timeout;
            fb=default_fb;
            if (length)
              if (p[0])
                framing_mode=p[0];
            if (length > 6)
              {
                /*
                  Note the delay and frame clipping boundaries.
                */
                p++; /* framing mode */
                while (*p && ((p-chunk) < (long) length))
                  p++;  /* frame name */
                p++;  /* frame name terminator */
                if ((p-chunk) < (long) (length-4))
                  {
                    int
                      change_delay,
                      change_timeout,
                      change_clipping;

                    change_delay=(*p++);
                    change_timeout=(*p++);
                    change_clipping=(*p++);
                    p++; /* change_sync */
                    if (change_delay)
                      {
                        frame_delay=(100*(mng_get_long(p))/ticks_per_second);
                        if (change_delay == 2)
                          default_frame_delay=frame_delay;
                        p+=4;
                      }
                    if (change_timeout)
                      {
                        frame_timeout=
                          (100*(mng_get_long(p))/ticks_per_second);
                        if (change_delay == 2)
                          default_frame_timeout=frame_timeout;
                        p+=4;
                      }
                    if (change_clipping)
                      {
                        fb=mng_read_box(previous_fb,p[0],&p[1]);
                        p+=17;
                        previous_fb=fb;
                      }
                    if (change_clipping == 2)
                      default_fb=fb;
                  }
              }
            clip=fb;
            clip=mng_minimum_box(fb,frame);
            subframe_width=(clip.right-clip.left);
            subframe_height=(clip.bottom-clip.top);
            /*
              Insert a background layer behind the frame if framing_mode is 4.
            */
            if (insert_layers && (framing_mode == 4) && (subframe_width) &&
                (subframe_height))
              {
                /*
                  Allocate next image structure.
                */
                if (GetPixels(image) != (PixelPacket *) NULL)
                  {
                    AllocateNextImage(image_info,image);
                    if (image->next == (Image *) NULL)
                      {
                        DestroyImages(image);
                        MngInfoFreeStruct(mng_info,&have_mng_structure);
                        return((Image *) NULL);
                      }
                    *image->next->blob=(*image->blob);
                    image=image->next;
                  }
                mng_info->image=image;
                if (term_chunk_found)
                  {
                    image->start_loop=True;
                    image->iterations=mng_iterations;
                    term_chunk_found=False;
                  }
                else
                    image->start_loop=False;
                image->columns= subframe_width;
                image->rows= subframe_height;
                image->page.width=subframe_width;
                image->page.height=subframe_height;
                image->page.x=clip.left;
                image->page.y=clip.top;
                image->background_color=mng_background_color;
                image->matte=False;
                image->delay=0;
                SetImage(image,OpaqueOpacity);
              }
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_CLIP,4))
          {
            unsigned int
              first_object,
              last_object;

            /*
              Read CLIP.
            */
            first_object=(p[0]<<8) | p[1];
            last_object=(p[2]<<8) | p[3];
            for (i=(int) first_object; i<= (int) last_object; i++)
            {
              if (mng_info->exists[i] && !mng_info->frozen[i])
                {
                  MngBox
                    box;

                  box=mng_info->object_clip[i];
                  mng_info->object_clip[i]=mng_read_box(box,p[4],&p[5]);
                }
            }
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_SAVE,4))
          {
            register int
              i;

            for (i=1; i<MNG_MAX_OBJECTS; i++)
              if (mng_info->exists[i])
                {
                 mng_info->frozen[i]=True;
#ifdef MNG_OBJECT_BUFFERS
                 if (mng_info->ob[i] != (MngBuffer *) NULL)
                    mng_info->ob[i]->frozen=True;
#endif
                }
            if (length)
              LiberateMemory((void **) &chunk);
            continue;
          }

        if (!memcmp(type,mng_DISC,4) || !memcmp(type,mng_SEEK,4))
          {
            register int
              i;

            /*
              Read DISC or SEEK.
            */
            if ((length == 0) || !memcmp(type,mng_SEEK,4))
              {
                for (i=1; i < MNG_MAX_OBJECTS; i++)
                  MngInfoDiscardObject(mng_info,i);
              }
            else
              {
                register long
                  j;

                for (j=0; j < (long) length; j+=2)
                {
                  i=p[j]<<8 | p[j+1];
                  MngInfoDiscardObject(mng_info,i);
                }
              }
            if (length)
              LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_MOVE,4))
          {
            unsigned long
              first_object,
              last_object;

            /*
              read MOVE
            */
            first_object=(p[0]<<8) | p[1];
            last_object=(p[2]<<8) | p[3];
            for (i=(long) first_object; i <= (long) last_object; i++)
            {
              if (mng_info->exists[i] && !mng_info->frozen[i])
                {
                  MngPair
                    new_pair,
                    old_pair;

                  old_pair.a=mng_info->x_off[i];
                  old_pair.b=mng_info->y_off[i];
                  new_pair=mng_read_pair(old_pair,p[4],&p[5]);
                  mng_info->x_off[i]=new_pair.a;
                  mng_info->y_off[i]=new_pair.b;
                }
            }
            LiberateMemory((void **) &chunk);
            continue;
          }

        if (!memcmp(type,mng_LOOP,4))
          {
            long loop_iters=1;
            loop_level=chunk[0];
            loops_active++;
            mng_info->loop_active[loop_level]=1;  /* mark loop active */
            /*
              Record starting point.
            */
            loop_iters = mng_get_long(&chunk[1]);
            if (loop_iters == 0)
              skipping_loop=loop_level;
            else
              {
                mng_info->loop_jump[loop_level]=TellBlob(image);
                mng_info->loop_count[loop_level]=loop_iters;
              }
            mng_info->loop_iteration[loop_level]=0;
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_ENDL,4))
          {
            loop_level=chunk[0];
            if (skipping_loop > 0)
              {
                if (skipping_loop == loop_level)
                  {
                    /*
                      Found end of zero-iteration loop.
                    */
                    skipping_loop=(-1);
                    loops_active--;
                    mng_info->loop_active[loop_level]=0;
                  }
              }
            else
              {
                if (mng_info->loop_active[loop_level] == 1)
                  {
                    mng_info->loop_count[loop_level]--;
                    mng_info->loop_iteration[loop_level]++;
                    if (mng_info->loop_count[loop_level] != 0)
                      (void) SeekBlob(image,mng_info->loop_jump[loop_level],
                        SEEK_SET);
                    else
                      {
                        short
                          last_level;

                        /*
                          Finished loop.
                        */
                        loops_active--;
                        mng_info->loop_active[loop_level]=0;
                        last_level=(-1);
                        for (i=0; i < loop_level; i++)
                          if (mng_info->loop_active[i] == 1)
                            last_level=(short) i;
                        loop_level=last_level;
                      }
                  }
              }
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (!memcmp(type,mng_CLON,4))
          {
            if (!mng_info->clon_warning)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "CLON is not implemented yet",image->filename);
            mng_info->clon_warning++;
          }
        if (!memcmp(type,mng_MAGN,4))
          {
            png_uint_16
              magn_first,
              magn_last,
              magn_mb,
              magn_ml,
              magn_mr,
              magn_mt,
              magn_mx,
              magn_my,
              magn_methx,
              magn_methy;

            if (length > 1)
              magn_first=(p[0] << 8) | p[1];
            else
              magn_first=0;
            if (length > 3)
              magn_last=(p[2] << 8) | p[3];
            else
              magn_last=magn_first;
#ifndef MNG_OBJECT_BUFFERS
            if (magn_first || magn_last)
              if (!mng_info->magn_warning)
                {
                  ThrowException(&image->exception,(ExceptionType)
                     DelegateWarning,
                     "MAGN is not implemented yet for nonzero objects",
                     image->filename);
                   mng_info->magn_warning++;
                }
#endif
            if (length > 4)
              magn_methx=p[4];
            else
              magn_methx=0;

            if (length > 6)
              magn_mx=(p[5] << 8) | p[6];
            else
              magn_mx=1;
            if (magn_mx == 0)
              magn_mx=1;

            if (length > 8)
              magn_my=(p[7] << 8) | p[8];
            else
              magn_my=magn_mx;
            if (magn_my == 0)
              magn_my=1;

            if (length > 10)
              magn_ml=(p[9] << 8) | p[10];
            else
              magn_ml=magn_mx;
            if (magn_ml == 0)
              magn_ml=1;

            if (length > 12)
              magn_mr=(p[11] << 8) | p[12];
            else
              magn_mr=magn_mx;
            if (magn_mr == 0)
              magn_mr=1;

            if (length > 14)
              magn_mt=(p[13] << 8) | p[14];
            else
              magn_mt=magn_my;
            if (magn_mt == 0)
              magn_mt=1;

            if (length > 16)
              magn_mb=(p[15] << 8) | p[16];
            else
              magn_mb=magn_my;
            if (magn_mb == 0)
              magn_mb=1;

            if (length > 17)
              magn_methy=p[17];
            else
              magn_methy=magn_methx;

            if (magn_methx > 5 || magn_methy > 5)
              if (!mng_info->magn_warning)
                {
                  ThrowException(&image->exception,(ExceptionType)
                     DelegateWarning,
                     "Unknown MAGN method in MNG datastream",
                     image->filename);
                   mng_info->magn_warning++;
                }
#ifdef MNG_OBJECT_BUFFERS
          /* Magnify existing objects in the range magn_first to magn_last */
#endif
            if (magn_first == 0 || magn_last == 0)
              {
                /* Save the magnification factors for object 0 */
                mng_info->magn_mb=magn_mb;
                mng_info->magn_ml=magn_ml;
                mng_info->magn_mr=magn_mr;
                mng_info->magn_mt=magn_mt;
                mng_info->magn_mx=magn_mx;
                mng_info->magn_my=magn_my;
                mng_info->magn_methx=magn_methx;
                mng_info->magn_methy=magn_methy;
              }
          }
        if (!memcmp(type,mng_PAST,4))
          {
            if (!mng_info->past_warning)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "PAST is not implemented yet",image->filename);
            mng_info->past_warning++;
          }
        if (!memcmp(type,mng_SHOW,4))
          {
            if (!mng_info->show_warning)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "SHOW is not implemented yet",image->filename);
            mng_info->show_warning++;
          }
        if (!memcmp(type,mng_sBIT,4))
          {
            if (length < 4)
              have_global_sbit=False;
            else
              {
                if (mng_info->global_sbit == (png_color_8p) NULL)
                  mng_info->global_sbit=
                    (png_color_8p) AcquireMemory(sizeof(png_color_8));
                mng_info->global_sbit->gray=p[0];
                mng_info->global_sbit->red=p[0];
                mng_info->global_sbit->green=p[1];
                mng_info->global_sbit->blue=p[2];
                mng_info->global_sbit->alpha=p[3];
                have_global_sbit=True;
             }
          }
        if (!memcmp(type,mng_pHYs,4))
          {
            if (length > 8)
              {
                mng_info->global_x_pixels_per_unit=mng_get_long(p);
                mng_info->global_y_pixels_per_unit=mng_get_long(&p[4]);
                mng_info->global_phys_unit_type=p[8];
                have_global_phys=True;
              }
            else
              have_global_phys=False;
          }
        if (!memcmp(type,mng_pHYg,4))
          {
            if (!mng_info->phyg_warning)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "pHYg is not implemented.",image->filename);
            mng_info->phyg_warning++;
          }
        if (!memcmp(type,mng_BASI,4))
          {
#ifdef MNG_BASI_SUPPORTED
            MngInfoPair
               pair;
#endif
            skip_to_iend=True;
            if (!mng_info->basi_warning)
              ThrowException(&image->exception,(ExceptionType) DelegateWarning,
                "BASI is not implemented yet",image->filename);
            mng_info->basi_warning++;
#ifdef MNG_BASI_SUPPORTED
            pair.a=0;
            pair.b=0;
            pair=mng_read_pair(pair,0,p);
            basi_width=(unsigned long) pair.a;
            basi_height=(unsigned long) pair.b;
            basi_color_type=p[8];
            basi_compression_method=p[9];
            basi_filter_type=p[10];
            basi_interlace_method=p[11];
            if (length > 11)
              basi_red=(p[12] << 8) & p[13];
            else
              basi_red=0;
            if (length > 13)
              basi_green=(p[14] << 8) & p[15];
            else
              basi_green=0;
            if (length > 15)
              basi_blue=(p[16] << 8) & p[17];
            else
              basi_blue=0;
            if (length > 17)
              basi_alpha=(p[18] << 8) & p[19];
            else
              {
                if (basi_sample_depth == 16)
                  basi_alpha=65535;
                else
                  basi_alpha=255;
              }
            if (length > 19)
              basi_viewable=p[20];
            else
              basi_viewable=0;
#endif
            LiberateMemory((void **) &chunk);
            continue;
          }
        if (memcmp(type,mng_IHDR,4))
          {
            if (length)
              LiberateMemory((void **) &chunk);
            continue;
          }
        mng_info->exists[object_id]=True;
        mng_info->viewable[object_id]=True;
        if (!mng_info->visible[object_id])
          {
            skip_to_iend=True;
            LiberateMemory((void **) &chunk);
            continue;
          }
        image_width=mng_get_long(p);
        image_height=mng_get_long(&p[4]);
        LiberateMemory((void **) &chunk);

        /*
          Insert a transparent background layer behind the entire animation
          if it is not full screen.
        */
        if (insert_layers && mng_type && first_mng_object)
          {
            if ((clip.left > 0) || (clip.top > 0) ||
                (image_width < mng_width) || (clip.right < (long) mng_width) ||
                (image_height < mng_height) || (clip.bottom < (long) mng_height))
              {
                if (GetPixels(image) != (PixelPacket *) NULL)
                  {
                    /*
                      Allocate next image structure.
                    */
                    AllocateNextImage(image_info,image);
                    if (image->next == (Image *) NULL)
                      {
                        DestroyImages(image);
                        MngInfoFreeStruct(mng_info,&have_mng_structure);
                        return((Image *) NULL);
                      }
                    *image->next->blob=(*image->blob);
                    image=image->next;
                  }
                mng_info->image=image;
                if (term_chunk_found)
                  {
                    image->start_loop=True;
                    image->iterations=mng_iterations;
                    term_chunk_found=False;
                  }
                else
                    image->start_loop=False;
                /*
                  Make a background rectangle.
                */
                image->delay=0;
                image->columns= mng_width;
                image->rows= mng_height;
                image->page.width=mng_width;
                image->page.height=mng_height;
                image->page.x=0;
                image->page.y=0;
                image->background_color=mng_background_color;
                SetImage(image,TransparentOpacity);
              }
          }
        /*
          Insert a background layer behind the upcoming image if
          framing_mode is 3, and we haven't already inserted one.
        */
        else if (insert_layers && (framing_mode == 3) && (subframe_width) &&
                (subframe_height) && (simplicity == 0 ||
                (simplicity & 0x08)))
          {
            if (GetPixels(image) != (PixelPacket *) NULL)
            {
              /*
                Allocate next image structure.
              */
              AllocateNextImage(image_info,image);
              if (image->next == (Image *) NULL)
                {
                  DestroyImages(image);
                  MngInfoFreeStruct(mng_info,&have_mng_structure);
                  return((Image *) NULL);
                }
              *image->next->blob=(*image->blob);
              image=image->next;
            }
            mng_info->image=image;
            if (term_chunk_found)
              {
                image->start_loop=True;
                image->iterations=mng_iterations;
                term_chunk_found=False;
              }
            else
                image->start_loop=False;
            image->delay=0;
            image->columns= subframe_width;
            image->rows= subframe_height;
            image->page.width=subframe_width;
            image->page.height=subframe_height;
            image->page.x=clip.left;
            image->page.y=clip.top;
            image->background_color=mng_background_color;
            image->matte=False;
            SetImage(image,OpaqueOpacity);
          }
        first_mng_object=False;
        /*
          Read the PNG image.
        */
        if (GetPixels(image) != (PixelPacket *) NULL)
          {
            /*
              Allocate next image structure.
            */
            AllocateNextImage(image_info,image);
            if (image->next == (Image *) NULL)
              {
                DestroyImages(image);
                MngInfoFreeStruct(mng_info,&have_mng_structure);
                return((Image *) NULL);
              }
            *image->next->blob=(*image->blob);
            image=image->next;
          }
        mng_info->image=image;
        MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
        if (term_chunk_found)
          {
            image->start_loop=True;
            term_chunk_found=False;
          }
        else
            image->start_loop=False;
        if (framing_mode == 1 || framing_mode == 3)
          {
            image->delay=frame_delay;
            frame_delay = default_frame_delay;
          }
        else
          image->delay=0;
        image->page.width=mng_width;
        image->page.height=mng_height;
        image->page.x=mng_info->x_off[object_id];
        image->page.y=mng_info->y_off[object_id];
        image->iterations=mng_iterations;
        /*
          Seek back to the beginning of the IHDR chunk's length field.
        */
        (void) SeekBlob(image,-((long) length+12),SEEK_CUR);
      }

    /*
      Allocate the PNG structures
    */
#ifdef PNG_USER_MEM_SUPPORTED
   ping=png_create_read_struct_2(PNG_LIBPNG_VER_STRING, image,
     PNGErrorHandler,PNGWarningHandler, NULL,
     (png_malloc_ptr) png_IM_malloc,(png_free_ptr) png_IM_free);
#else
    ping=png_create_read_struct(PNG_LIBPNG_VER_STRING,image,
      PNGErrorHandler,PNGWarningHandler);
#endif
    if (ping == (png_struct *) NULL)
      ThrowReaderException((ExceptionType) ResourceLimitWarning,
        "Memory allocation failed",image);
    ping_info=png_create_info_struct(ping);
    if (ping_info == (png_info *) NULL)
      {
        png_destroy_read_struct(&ping,(png_info **) NULL,(png_info **) NULL);
        ThrowReaderException((ExceptionType) ResourceLimitWarning,
          "Memory allocation failed", image)
      }
    end_info=png_create_info_struct(ping);
    if (end_info == (png_info *) NULL)
      {
        png_destroy_read_struct(&ping,&ping_info,(png_info **) NULL);
        ThrowReaderException((ExceptionType) ResourceLimitWarning,
          "Memory allocation failed", image)
      }
    png_pixels=(unsigned char *) NULL;
    scanlines=(unsigned char **) NULL;
    if (setjmp(ping->jmpbuf))
      {
        /*
          PNG image is corrupt.
        */
        png_destroy_read_struct(&ping,&ping_info,&end_info);
        if (scanlines != (unsigned char **) NULL)
          LiberateMemory((void **) &scanlines);
        if (png_pixels != (unsigned char *) NULL)
          LiberateMemory((void **) &png_pixels);
        CloseBlob(image);
        if ((image->columns == 0) || (image->rows == 0))
          {
            DestroyImage(image);
            MngInfoFreeStruct(mng_info,&have_mng_structure);
            return((Image *) NULL);
          }
        MngInfoFreeStruct(mng_info,&have_mng_structure);
        return(image);
      }
    /*
      Prepare PNG for reading.
    */
    image_found++;
    if (LocaleCompare(image_info->magick,"MNG") == 0)
      {
        png_set_sig_bytes(ping,8);
#if defined(PNG_MNG_FEATURES_SUPPORTED)
        png_permit_mng_features(ping,PNG_ALL_MNG_FEATURES);
        png_set_read_fn(ping,image,png_get_data);
#else
#if defined(PNG_READ_EMPTY_PLTE_SUPPORTED)
        png_permit_empty_plte(ping,True);
        png_set_read_fn(ping,image,png_get_data);
#  else
        mng_info->image=image;
        mng_info->bytes_in_read_buffer=0;
        mng_info->found_empty_plte=False;
        mng_info->have_saved_bkgd_index=False;
        png_set_read_fn(ping,mng_info,mng_get_data);
#  endif
#endif
      }
    else
      png_set_read_fn(ping,image,png_get_data);

#if defined(PNG_USE_PNGGCCRD) && defined(PNG_ASSEMBLER_CODE_SUPPORTED) \
  && (PNG_LIBPNG_VER >= 10200)
    /* Disable thread-unsafe features of pnggccrd */
    if (png_access_version() >= 10200)
    {
      png_uint_32 mmx_disable_mask = 0;
      png_uint_32 asm_flags;

      mmx_disable_mask |= ( PNG_ASM_FLAG_MMX_READ_COMBINE_ROW  \
                          | PNG_ASM_FLAG_MMX_READ_FILTER_SUB   \
                          | PNG_ASM_FLAG_MMX_READ_FILTER_AVG   \
                          | PNG_ASM_FLAG_MMX_READ_FILTER_PAETH );
      asm_flags = png_get_asm_flags(png_ptr);
      png_set_asm_flags(png_ptr, asm_flags & ~mmx_disable_mask);
    }
#endif

    png_read_info(ping,ping_info);
    image->depth=ping_info->bit_depth;
    if (ping_info->bit_depth < 8)
      {
        if ((ping_info->color_type == PNG_COLOR_TYPE_PALETTE))
          {
            png_set_packing(ping);
            image->depth=8;
          }
      }
#if (PNG_LIBPNG_VER > 10008) && defined(PNG_READ_iCCP_SUPPORTED)
    if (ping_info->valid & PNG_INFO_iCCP)
      {
        int
          compression;

        png_charp
          info,
          name;

        (void) png_get_iCCP(ping,ping_info,&name,(int *) &compression,&info,
          (png_uint_32 *) &image->color_profile.length);

        if (image->color_profile.name)
           LiberateMemory((void **) &image->color_profile.name);
        if (image->color_profile.info)
            LiberateMemory((void **) &image->color_profile.info);
        if (image->color_profile.length)
          {
#ifdef PNG_FREE_ME_SUPPORTED
            image->color_profile.name=AllocateString("icm");
            image->color_profile.info=(unsigned char *) info;
            png_data_freer(ping, ping_info, PNG_USER_WILL_FREE_DATA,
               PNG_FREE_ICCP);
#else
            /* libpng will destroy name and info so we must copy them now. */
            image->color_profile.info=(unsigned char *) AcquireMemory(length);
            if (image->color_profile.info == (unsigned char *) NULL)
              {
                 MagickWarning(OptionWarning, "Unable to allocate ICC profile",
                    "Memory allocation failed");
                 image->color_profile.length=0;
              }
            else
              {
                 (void) memcpy(image->color_profile.info,(unsigned char *) info,length);
                 image->color_profile.name=AllocateString("icm");
                 /* Note that the PNG iCCP profile name gets lost. */
              }
#endif
        }

      }
#endif
#if defined(PNG_READ_sRGB_SUPPORTED)
    {
      int
        intent;

      if (have_global_srgb)
        image->rendering_intent=(RenderingIntent)
          (mng_info->global_srgb_intent+1);
      if (png_get_sRGB(ping,ping_info,&intent))
        image->rendering_intent=(RenderingIntent) (intent+1);
    }
#endif
    {
       double
          file_gamma;

       if (have_global_gama)
         image->gamma=mng_info->global_gamma;
       if (png_get_gAMA(ping,ping_info,&file_gamma))
         image->gamma=(float) file_gamma;
    }
    if (have_global_chrm)
      image->chromaticity=mng_info->global_chrm;
    if (ping_info->valid & PNG_INFO_cHRM)
      {
        (void) png_get_cHRM(ping,ping_info,
          &image->chromaticity.white_point.x,
          &image->chromaticity.white_point.y,
          &image->chromaticity.red_primary.x,
          &image->chromaticity.red_primary.y,
          &image->chromaticity.green_primary.x,
          &image->chromaticity.green_primary.y,
          &image->chromaticity.blue_primary.x,
          &image->chromaticity.blue_primary.y);
      }
    if (image->rendering_intent)
      {
        image->gamma=0.45455;
        image->chromaticity.red_primary.x=0.6400f;
        image->chromaticity.red_primary.y=0.3300f;
        image->chromaticity.red_primary.z=1.0000f;
        image->chromaticity.green_primary.x=0.3000f;
        image->chromaticity.green_primary.y=0.6000f;
        image->chromaticity.green_primary.z=1.0000f;
        image->chromaticity.blue_primary.x=0.1500f;
        image->chromaticity.blue_primary.y=0.0600f;
        image->chromaticity.blue_primary.z=1.0000f;
        image->chromaticity.white_point.x=0.3127f;
        image->chromaticity.white_point.y=0.3290f;
        image->chromaticity.white_point.z=1.0000f;
      }
    if (have_global_gama || image->rendering_intent)
      ping_info->valid|=PNG_INFO_gAMA;
    if (have_global_chrm || image->rendering_intent)
      ping_info->valid|=PNG_INFO_cHRM;
    if (ping_info->valid & PNG_INFO_pHYs)
      {
        int
          unit_type;

        png_uint_32
          x_resolution,
          y_resolution;

        /*
          Set image resolution.
        */
        png_get_pHYs(ping,ping_info,&x_resolution,&y_resolution,&unit_type);
        image->x_resolution=(float) x_resolution;
        image->y_resolution=(float) y_resolution;
        if (unit_type == PNG_RESOLUTION_METER)
          {
            image->units=PixelsPerCentimeterResolution;
            image->x_resolution=(double) x_resolution/100.0;
            image->y_resolution=(double) y_resolution/100.0;
          }
      }
    else
      {
        if (have_global_phys)
          {
            image->x_resolution=(float) mng_info->global_x_pixels_per_unit;
            image->y_resolution=(float) mng_info->global_y_pixels_per_unit;
            if (mng_info->global_phys_unit_type == PNG_RESOLUTION_METER)
              {
                image->units=PixelsPerCentimeterResolution;
                image->x_resolution=(double)
                  mng_info->global_x_pixels_per_unit/100.0;
                image->y_resolution=(double)
                  mng_info->global_y_pixels_per_unit/100.0;
              }
            ping_info->valid|=PNG_INFO_pHYs;
          }
      }
    if (ping_info->valid & PNG_INFO_PLTE)
      {
        int
          number_colors;

        png_colorp
          palette;

        (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);
        if (number_colors == 0 && ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          {
            if (global_plte_length)
              {
                png_set_PLTE(ping,ping_info,mng_info->global_plte,
                  (int) global_plte_length);
                if (!(ping_info->valid & PNG_INFO_tRNS))
                  if (global_trns_length)
                    {
                      if (global_trns_length > global_plte_length)
                        ThrowException(&image->exception,(ExceptionType)
                          DelegateWarning,
                          "global tRNS has more entries than global PLTE",
                          image_info->filename);
                      png_set_tRNS(ping,ping_info,mng_info->global_trns,
                        (int) global_trns_length,NULL);
                    }
#if defined(PNG_READ_bKGD_SUPPORTED)
                if (
#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
                     mng_info->have_saved_bkgd_index ||
#endif
                     ping_info->valid & PNG_INFO_bKGD)
                      {
                        png_color_16
                           background;

#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
                        if (mng_info->have_saved_bkgd_index)
                          background.index=mng_info->saved_bkgd_index;
                        else
#endif
                          background.index=ping_info->background.index;
                        background.red=(png_uint_16)
                          mng_info->global_plte[background.index].red;
                        background.green= (png_uint_16)
                          mng_info->global_plte[background.index].green;
                        background.blue=(png_uint_16)
                          mng_info->global_plte[background.index].blue;
                        png_set_bKGD(ping,ping_info,&background);
                      }
#endif
                  }
                else
                  ThrowException(&image->exception,(ExceptionType)
                    DelegateWarning,
                    "No global PLTE in file",image_info->filename);
              }
          }

#if defined(PNG_READ_bKGD_SUPPORTED)
    if (have_global_bkgd && !(ping_info->valid & PNG_INFO_bKGD))
      {
#if (QuantumDepth == 16)
        image->background_color=mng_global_bkgd;
#else
        image->background_color.red=XDownScale(mng_global_bkgd.red);
        image->background_color.green=XDownScale(mng_global_bkgd.green);
        image->background_color.blue=XDownScale(mng_global_bkgd.blue);
#endif
      }
    if (ping_info->valid & PNG_INFO_bKGD)
      {
        /*
          Set image background color.
        */
        if (ping_info->bit_depth <= QuantumDepth)
          {
            image->background_color.red=ping_info->background.red;
            image->background_color.green=ping_info->background.green;
            image->background_color.blue=ping_info->background.blue;
          }
        else
          {
            image->background_color.red=XDownScale(ping_info->background.red);
            image->background_color.green=
              XDownScale(ping_info->background.green);
            image->background_color.blue=XDownScale(ping_info->background.blue);
          }
      }
#endif
    if (ping_info->valid & PNG_INFO_tRNS)
      {
        int
          scale;

        scale=(int) MaxRGB/((int) ((1<<ping_info->bit_depth)-1));
        if (scale < 1)
           scale=1;
        /*
          Image has a transparent background.
        */
        transparent_color.red=(int) (ping_info->trans_values.red*scale);
        transparent_color.green=(int) (ping_info->trans_values.green*scale);
        transparent_color.blue=(int) (ping_info->trans_values.blue*scale);
        transparent_color.opacity=(int) (ping_info->trans_values.gray*scale);
        if (ping_info->color_type == PNG_COLOR_TYPE_GRAY)
          {
            transparent_color.red=transparent_color.opacity;
            transparent_color.green=transparent_color.opacity;
            transparent_color.blue=transparent_color.opacity;
          }
      }
#if defined(PNG_READ_sBIT_SUPPORTED)
    if (have_global_sbit)
      {
        int
           not_valid;
        not_valid=!ping_info->valid;
        if (not_valid & PNG_INFO_sBIT)
          png_set_sBIT(ping,ping_info,mng_info->global_sbit);
      }
#endif
    png_read_update_info(ping,ping_info);
    /*
      Initialize image structure.
    */
    image_box.left=0;
    image_box.right=(long) ping_info->width;
    image_box.top=0;
    image_box.bottom=(long) ping_info->height;
    if (mng_type == 0)
      {
        mng_width=ping_info->width;
        mng_height=ping_info->height;
        frame=image_box;
        clip=image_box;
      }
    image->compression=ZipCompression;
    image->columns= ping_info->width;
    image->rows= ping_info->height;
    if ((ping_info->color_type == PNG_COLOR_TYPE_PALETTE) ||
        (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ||
        (ping_info->color_type == PNG_COLOR_TYPE_GRAY))
      {
        image->storage_class=PseudoClass;
        image->colors=1 << ping_info->bit_depth;
        if (image->colors>MaxRGB+1)
          image->colors=MaxRGB+1;
        if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          {
            int
              number_colors;

            png_colorp
              palette;

            (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);
            image->colors=number_colors;
          }
      }
    png_pixels=(unsigned char *)
      AcquireMemory(ping_info->rowbytes*image->rows*sizeof(Quantum));
    scanlines=(unsigned char **)
      AcquireMemory(image->rows*sizeof(unsigned char *));
    if ((png_pixels == (unsigned char *) NULL) ||
        (scanlines == (unsigned char **) NULL))
      ThrowReaderException((ExceptionType) ResourceLimitWarning,
        "Memory allocation failed",image);

    if (image->storage_class==PseudoClass)
      {
        /*
          Initialize image colormap.
        */
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException((ExceptionType) ResourceLimitWarning,
            "Memory allocation failed",image);
        if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          {
            int
              number_colors;

            png_colorp
              palette;

            (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);
            for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=UpScale(palette[i].red);
              image->colormap[i].green=UpScale(palette[i].green);
              image->colormap[i].blue=UpScale(palette[i].blue);
            }
          }
        else
          {
            long
              scale;

            scale=MaxRGB/((int) ((1<<ping_info->bit_depth)-1));
            if (scale < 1)
               scale=1;
            for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=(Quantum) (i*scale);
              image->colormap[i].green=(Quantum) (i*scale);
              image->colormap[i].blue=(Quantum) (i*scale);
            }
         }
      }
    /*
      Read image scanlines.
    */
    for (i=0; i < (long) image->rows; i++)
      scanlines[i]=png_pixels+(i*ping_info->rowbytes);
    png_read_image(ping,scanlines);
    png_read_end(ping,ping_info);
    /*
      Convert PNG pixels to pixel packets.
    */
    if (image->storage_class == DirectClass)
      {
        /*
          Convert image to DirectClass pixel packets.
        */
#if (QuantumDepth == 8)
        int
          depth;

        depth=(long) image->depth;
#endif
        image->matte=((ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
            (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ||
            (ping_info->valid & PNG_INFO_tRNS));

        for (y=0; y < (long) image->rows; y++)
        {
          if (!SetImagePixels(image,0,y,image->columns,1))
            break;
#if (QuantumDepth == 8)
          if (depth == 16)
            {
              register Quantum
                *p,
                *r;

              r=scanlines[y];
              p=r;
              for (x=0; x < (long) image->columns; x++)
              {
                if (ping_info->color_type == PNG_COLOR_TYPE_GRAY)
                  {
                    *r++ = *p++;
                    p++;
                    if ((ping_info->valid & PNG_INFO_tRNS) &&
                       (((*(p-2)<<8)|*(p-1))==transparent_color.opacity))
                      {
                         /* Cheap transparency */
                         *r++ = TransparentOpacity;
                      }
                    else
                         *r++ = OpaqueOpacity;
                  }
                if (ping_info->color_type == PNG_COLOR_TYPE_RGB)
                  {
                    *r++ = *p++;
                    p++;
                    *r++ = *p++;
                    p++;
                    *r++ = *p++;
                    p++;
                    if ((ping_info->valid & PNG_INFO_tRNS) &&
                         (((*(p-6)<<8)|*(p-5))==transparent_color.red) &&
                         (((*(p-4)<<8)|*(p-3))==transparent_color.green) &&
                         (((*(p-2)<<8)|*(p-1))==transparent_color.blue))
                      {
                         /* Cheap transparency */
                         *r++ = TransparentOpacity;
                      }
                    else
                         *r++ = OpaqueOpacity;
                  }
                if (ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
                {
                  *r++ = *p++;
                  p++;
                  *r++ = *p++;
                  p++;
                  *r++ = *p++;
                  p++;
                }
                if (ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
                    ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                {
                  *r++ = *p++;
                  p++;
                }
              }
            }
          if (depth == 8 && ping_info->color_type == PNG_COLOR_TYPE_GRAY)
            (void) PushImagePixels(image,(QuantumType) GrayQuantum,
              scanlines[y]);
          if (ping_info->color_type == PNG_COLOR_TYPE_GRAY ||
              ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            {
              image->depth=8;
              (void) PushImagePixels(image,(QuantumType) GrayAlphaQuantum,
                scanlines[y]);
              image->depth=depth;
            }
          else if (depth == 8 && ping_info->color_type == PNG_COLOR_TYPE_RGB)
             (void) PushImagePixels(image,(QuantumType) RGBQuantum,
                scanlines[y]);
          else if (ping_info->color_type == PNG_COLOR_TYPE_RGB ||
                ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
            {
              image->depth=8;
              (void) PushImagePixels(image,(QuantumType) RGBAQuantum,
                scanlines[y]);
              image->depth=depth;
            }
          else if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
              (void) PushImagePixels(image,(QuantumType) IndexQuantum,
                scanlines[y]);
#else
          if (ping_info->color_type == PNG_COLOR_TYPE_GRAY)
            (void) PushImagePixels(image,(QuantumType) GrayQuantum,
                scanlines[y]);
          else if (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            (void) PushImagePixels(image,(QuantumType) GrayAlphaQuantum,
                scanlines[y]);
          else if (ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
            (void) PushImagePixels(image,(QuantumType) RGBAQuantum,
                scanlines[y]);
          else if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
            (void) PushImagePixels(image,(QuantumType) IndexQuantum,
                scanlines[y]);
          else
            (void) PushImagePixels(image,(QuantumType) RGBQuantum,
                scanlines[y]);
#endif
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
      }
    else /* image->storage_class != DirectClass */
      {
        Quantum
          *quantum_scanline;

        register Quantum
          *r;

        /*
          Convert grayscale image to PseudoClass pixel packets.
        */
        quantum_scanline=(Quantum *) AcquireMemory((ping_info->color_type == 4
          ? 2 : 1) * image->columns*sizeof(Quantum));
        if (quantum_scanline == (Quantum *) NULL)
          ThrowReaderException((ExceptionType) ResourceLimitWarning,
            "Memory allocation failed",image);
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          p=scanlines[y];
          r=quantum_scanline;
          switch (ping_info->bit_depth)
          {
            case 1:
            {
              register int
                bit;

              for (x=0; x < ((long) image->columns-7); x+=8)
              {
                for (bit=7; bit >= 0; bit--)
                  *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                p++;
              }
              if ((image->columns % 8) != 0)
                {
                  for (bit=7; bit >= (long) (8-(image->columns % 8)); bit--)
                    *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                }
              break;
            }
            case 2:
            {
              for (x=0; x < ((long) image->columns-3); x+=4)
              {
                *r++=(*p >> 6) & 0x03;
                *r++=(*p >> 4) & 0x03;
                *r++=(*p >> 2) & 0x03;
                *r++=(*p++) & 0x03;
              }
              if ((image->columns % 4) != 0)
                {
                  for (i=3; i >= (long) (4-(image->columns % 4)); i--)
                    *r++=(*p >> (i*2)) & 0x03;
                }
              break;
            }
            case 4:
            {
              for (x=0; x < ((long) image->columns-1); x+=2)
              {
                *r++=(*p >> 4) & 0x0f;
                *r++=(*p++) & 0x0f;
              }
              if ((image->columns % 2) != 0)
                *r++=(*p++ >> 4) & 0x0f;
              break;
            }
            case 8:
            {
              for (x=0; x < (long) image->columns; x++)
              {
                *r++=*p++;
                if (ping_info->color_type == 4)
                  {
                    /* In image.h, OpaqueOpacity is 0
                     * TransparentOpacity is MaxRGB
                     * In a PNG datastream, Opaque is MaxRGB
                     * and Transparent is 0.
                     */
                    q->opacity=UpScale(255-(*p++));
                    q++;
                  }
              }
              break;
            }
            case 16:
            {
              for (x=0; x < (long) image->columns; x++)
              {
#if (QuantumDepth == 16)
                *r=((*p++) << 8);
                *r++|=(*p++);
                if (ping_info->color_type == 4)
                  {
                    q->opacity=((*p++) << 8);
                    q->opacity|=(*p++);
                    q->opacity=(Quantum) (MaxRGB-q->opacity);
                    q++;
                  }
#else
                *r++=(*p++);
                p++; /* strip low byte */
                if (ping_info->color_type == 4)
                  {
                    q->opacity=(Quantum) (MaxRGB-(*p++));
                    p++;
                    q++;
                  }
#endif
              }
              break;
            }
            default:
              break;
          }
          /*
            Transfer image scanline.
          */
          r=quantum_scanline;
          for (x=0; x < (long) image->columns; x++)
            indexes[x]=(*r++);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
        LiberateMemory((void **) &quantum_scanline);
      }
    if (image->storage_class == PseudoClass)
      SyncImage(image);
    if (ping_info->valid & PNG_INFO_tRNS)
      {
        ClassType
          storage_class;

        /*
          Image has a transparent background.
        */
        storage_class=image->storage_class;
        image->matte=True;
        for (y=0; y < (long) image->rows; y++)
        {
          image->storage_class=storage_class;
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            IndexPacket
              index;

            q->opacity=OpaqueOpacity;
            if (storage_class == PseudoClass)
              {
                index=indexes[x];
                if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
                  {
                    if (index < ping_info->num_trans)
                      q->opacity=UpScale(255-ping_info->trans[index]);
                  }
                else if (ping_info->color_type == PNG_COLOR_TYPE_GRAY)
                  {
                    q->red=image->colormap[index].red;
                    q->green=image->colormap[index].green;
                    q->blue=image->colormap[index].blue;
                    if (q->red == transparent_color.opacity)
                      q->opacity=TransparentOpacity;
                  }
              }
            else
              if (q->red == transparent_color.red &&
                  q->green == transparent_color.green &&
                  q->blue == transparent_color.blue)
                 q->opacity=TransparentOpacity;
            q++;
          }
          image->storage_class=DirectClass;
          if (!SyncImagePixels(image))
            break;
        }
      }
#if (QuantumDepth == 8)
    if (image->depth > 8)
      image->depth = 8;
#endif
    if (png_get_text(ping,ping_info,&text,&num_text) != 0)
      for (i=0; i < (long) num_text; i++)
      {

        /* Check for a profile */

        if (!memcmp(text[i].key, "Raw profile type ",17))
            (void) png_read_raw_profile(image,image_info,text,(int) i);
        else
          {
            char
              *value;

            length=text[i].text_length;
            value=(char *) AcquireMemory(length+1);
            if (value == (char *) NULL)
              {
                ThrowException(&image->exception,(ExceptionType)
                   ResourceLimitWarning,
                  "Unable to read text chunk","Memory allocation failed");
                break;
              }
            *value='\0';
            (void) strncat(value,text[i].text,length);
            value[length]='\0';
            (void) SetImageAttribute(image,text[i].key,value);
            LiberateMemory((void **) &value);
          }
      }
#ifdef MNG_OBJECT_BUFFERS
    /*
      Store the object if necessary.
    */
    if (object_id && !mng_info->frozen[object_id])
      {
        if (mng_info->ob[object_id] == (MngBuffer *) NULL)
          {
            /*
              create a new object buffer.
            */
            mng_info->ob[object_id]=(MngBuffer *)
              AcquireMemory(sizeof(MngBuffer));
            if (mng_info->ob[object_id] != (MngBuffer *) NULL)
              {
                mng_info->ob[object_id]->image=(Image *) NULL;
                mng_info->ob[object_id]->reference_count=1;
              }
          }
        if ((mng_info->ob[object_id] == (MngBuffer *) NULL) ||
            mng_info->ob[object_id]->frozen)
          {
            if (mng_info->ob[object_id] == (MngBuffer *) NULL)
              ThrowException(&image->exception,(ExceptionType)
                ResourceLimitWarning,
                "Memory allocation of MNG object buffer failed",
                image->filename);
            if (mng_info->ob[object_id]->frozen)
              ThrowException(&image->exception,(ExceptionType)
                 ResourceLimitWarning,
                "Cannot overwrite frozen MNG object buffer",image->filename);
          }
        else
          {
            png_uint_32
              width,
              height;

            int
              bit_depth,
              color_type,
              interlace_method,
              compression_method,
              filter_method;

            if (mng_info->ob[object_id]->image != (Image *) NULL)
              DestroyImage(mng_info->ob[object_id]->image);
            mng_info->ob[object_id]->image=CloneImage(image,0,0,True);
            if (mng_info->ob[object_id]->image != (Image *) NULL)
              mng_info->ob[object_id]->image->file=(FILE *) NULL;
            else
              ThrowException(&image->exception,(ExceptionType)
                ResourceLimitWarning,
                "Cloning image for object buffer failed",image->filename);
            png_get_IHDR(ping,ping_info,&width,&height,&bit_depth,&color_type,
              &interlace_method,&compression_method,&filter_method);
            mng_info->ob[object_id]->width=width;
            mng_info->ob[object_id]->height=height;
            mng_info->ob[object_id]->color_type=color_type;
            mng_info->ob[object_id]->sample_depth=bit_depth;
            mng_info->ob[object_id]->interlace_method=interlace_method;
            mng_info->ob[object_id]->compression_method=compression_method;
            mng_info->ob[object_id]->filter_method=filter_method;
            if (ping_info->valid & PNG_INFO_PLTE)
              {
                int
                  number_colors;

                png_colorp
                  plte;

                /*
                  Copy the PLTE to the object buffer.
                */
                png_get_PLTE(ping,ping_info,&plte,&number_colors);
                mng_info->ob[object_id]->plte_length = number_colors;
                for (i=0; i<number_colors; i++)
                {
                  mng_info->ob[object_id]->plte[i]=plte[i];
                }
              }
            else
                mng_info->ob[object_id]->plte_length=0;
          }
      }
#endif
    /*
      Free memory.
    */
    png_destroy_read_struct(&ping,&ping_info,&end_info);
    LiberateMemory((void **) &png_pixels);
    LiberateMemory((void **) &scanlines);
    if (mng_type)
      {
        MngBox
          crop_box;

        if (mng_info->magn_methx || mng_info->magn_methy)
          {
            png_uint_32
               magnified_height,
               magnified_width;

            if (mng_info->magn_methx == 1)
              {
                magnified_width=mng_info->magn_ml;
                if (image->columns > 1)
                   magnified_width += mng_info->magn_mr;
                if (image->columns > 2)
                   magnified_width += (image->columns-2)*(mng_info->magn_mx);
              }
            else
              {
                magnified_width=image->columns;
                if (image->columns > 1)
                   magnified_width += mng_info->magn_ml-1;
                if (image->columns > 2)
                   magnified_width += mng_info->magn_mr-1;
                if (image->columns > 3)
                   magnified_width += (image->columns-3)*(mng_info->magn_mx-1);
              }
            if (mng_info->magn_methy == 1)
              {
                magnified_height=mng_info->magn_mt;
                if (image->rows > 1)
                   magnified_height += mng_info->magn_mb;
                if (image->rows > 2)
                   magnified_height += (image->rows-2)*(mng_info->magn_my);
              }
            else
              {
                magnified_height=image->rows;
                if (image->rows > 1)
                   magnified_height += mng_info->magn_mt-1;
                if (image->rows > 2)
                   magnified_height += mng_info->magn_mb-1;
                if (image->rows > 3)
                   magnified_height += (image->rows-3)*(mng_info->magn_my-1);
              }
            if (magnified_height > image->rows ||
                magnified_width > image->columns)
              {
                Image *
                  large_image;

                int
                  yy;

                long
                  m,
                  y;
/*
                register IndexPacket
                  *indexes;
*/

                register long
                  i,
                  x;

                register PixelPacket
                  *n,
                  *p,
                  *q;

                PixelPacket
                  *next,
                  *prev;

                size_t
                  length;

                png_uint_16
                  magn_methx,
                  magn_methy;

                if (GetPixels(image) != (PixelPacket *) NULL)
                  {
                    AllocateNextImage(image_info,image);
                    if (image->next == (Image *) NULL)
                      {
                         DestroyImages(image);
                         MngInfoFreeStruct(mng_info,&have_mng_structure);
                         ThrowReaderException((ExceptionType)
                            ResourceLimitWarning,
                           "Memory allocation failed while magnifying",image)
                      }
                   }

                large_image=image->next;
                *large_image->blob=(*image->blob);
                large_image->columns= magnified_width;
                large_image->rows= magnified_height;

                magn_methx=mng_info->magn_methx;
                magn_methy=mng_info->magn_methy;

                if (image->matte)
                   SetImage(large_image,TransparentOpacity);
                else
                  {
                    SetImage(large_image,OpaqueOpacity);
                    if (magn_methx == 4)
                      magn_methx = 2;
                    if (magn_methx == 5)
                      magn_methx = 3;
                    if (magn_methy == 4)
                      magn_methy = 2;
                    if (magn_methy == 5)
                      magn_methy = 3;
                  }

                /* magnify the rows into the right side of the large image */

                m=mng_info->magn_mt;
                yy=0;
                length=(size_t) (image->columns*sizeof(PixelPacket));
                next=(PixelPacket *) AcquireMemory(length);
                prev=(PixelPacket *) AcquireMemory(length);
                if ((prev == (PixelPacket *) NULL) ||
                    (next == (PixelPacket *) NULL))
                  {
                     DestroyImages(image);
                     MngInfoFreeStruct(mng_info,&have_mng_structure);
                     ThrowReaderException((ExceptionType) ResourceLimitWarning,
                       "Memory allocation failed while magnifying",image)
                  }
                n=GetImagePixels(image,0,0,image->columns,1);
                (void) memcpy(next,n,length);
                for (y=0; y < (long) image->rows; y++)
                {
                  if (y == 0)
                    m=mng_info->magn_mt;
                  else if (magn_methy > 1 && y == (long) image->rows-2)
                    m=mng_info->magn_mb;
                  else if (magn_methy <= 1 && y == (long) image->rows-1)
                    m=mng_info->magn_mb;
                  else if (magn_methy > 1 && y == (long) image->rows-1)
                    m=1;
                  else
                    m=mng_info->magn_my;
                  n=prev;
                  prev=next;
                  next=n;
                  if (y < (long) image->rows-1)
                    {
                      n=GetImagePixels(image,0,y+1,image->columns,1);
                      (void) memcpy(next,n,length);
                    }
                  for (i=0; i<m; i++, yy++)
                  {
                    assert(yy < (long) large_image->rows);
                    p=prev;
                    n=next;
                    q=SetImagePixels(large_image,0,yy,large_image->columns,1);
                    q+=(large_image->columns-image->columns);
                    for (x=0; x < (long) image->columns; x++)
                    {
                      /* TO DO: get color as function of indexes[x] */
                      /*
                      if (image->storage_class == PseudoClass)
                        {
                        }
                      */

                      if (magn_methy <= 1)
                        {
                          *q=(*p); /* replicate previous */
                        }
                      else if (magn_methy == 2 || magn_methy == 4)
                        {
                          if (i == 0)
                             *q=(*p);
                          else
                            {
                              /* Interpolate */
                              (*q).red=(Quantum) (((long) (2*i*((*n).red
                                 -(*p).red)+m))/((long) (m*2))+(*p).red);
                              (*q).green=(Quantum) (((long) (2*i*((*n).green
                                 -(*p).green)+m))/((long) (m*2))+(*p).green);
                              (*q).blue=(Quantum) (((long) (2*i*((*n).blue
                                 -(*p).blue)+m))/((long) (m*2))+(*p).blue);
                              if (image->matte)
                                 (*q).opacity=(Quantum) (((long)
                                 (2*i*((*n).opacity-(*p).opacity)+m))
                                 /((long) (m*2))+(*p).opacity);
                            }
                          if (magn_methy == 4)
                            {
                              /* Replicate nearest */
                              if (i <= ((m+1)<<1))
                                 (*q).opacity=(*p).opacity+0;
                              else
                                 (*q).opacity=(*n).opacity+0;
                            }
                        }
                      else /* if (magn_methy == 3 || magn_methy == 5) */
                        {
                          /* Replicate nearest */
                          if (i <= ((m+1)<<1))
                             *q=(*p);
                          else
                             *q=(*n);
                          if (magn_methy == 5)
                            {
                              (*q).opacity=(Quantum) (((long) (2*i*((*n).opacity
                                 -(*p).opacity)+m))/((long) (m*2))+(*p).opacity);
                            }
                        }
                      n++;
                      q++;
                      p++;
                    } /* x */
                    if (!SyncImagePixels(large_image))
                      break;
                  } /* i */
                } /* y */
                LiberateMemory((void **) &prev);
                LiberateMemory((void **) &next);

                length=image->columns;
                large_image->previous=image->previous;
                if (image->previous)
                   image->previous->next=large_image;
                image->orphan=True;
                image->file=(FILE *) NULL;
                image->blob->mapped=False;
                DestroyImage(image);
                image=large_image;
                mng_info->image=image;

                /* magnify the columns */

                for (y=0; y < (long) image->rows; y++)
                {
                  q=GetImagePixels(image,0,y,image->columns,1);
                  p=q+(image->columns-length);
                  n=p+1;
                  for (x=(long) (image->columns-length);
                    x < (long) image->columns; x++)
                  {
                    if (x == (long) (image->columns-length))
                      m=mng_info->magn_ml;
                    else if (magn_methx > 1 && x == (long) image->columns-2)
                      m=mng_info->magn_mr;
                    else if (magn_methx <= 1 && x == (long) image->columns-1)
                      m=mng_info->magn_mr;
                    else if (magn_methx > 1 && x == (long) image->columns-1)
                      m=1;
                    else
                      m=mng_info->magn_mx;
                    for (i=0; i<m; i++)
                    {
                      if (magn_methx <= 1)
                        {
                          /* replicate previous */
                          *q=(*p);
                        }
                      else if (magn_methx == 2 || magn_methx == 4)
                        {
                          if (i == 0)
                            *q=(*p);
                          else
                            {
                              /* Interpolate */
                              (*q).red=(Quantum) ((2*i*((*n).red-(*p).red)+m)
                                 /((long) (m*2))+(*p).red);
                              (*q).green=(Quantum) ((2*i*((*n).green-(*p).green)
                                 +m)/((long) (m*2))+(*p).green);
                              (*q).blue=(Quantum) ((2*i*((*n).blue-(*p).blue)+m)
                                 /((long) (m*2))+(*p).blue);
                              if (image->matte)
                                 (*q).opacity=(Quantum) ((2*i*((*n).opacity
                                   -(*p).opacity)+m)/((long) (m*2))+(*p).opacity);
                            }
                          if (magn_methx == 4)
                            {
                              /* Replicate nearest */
                              if (i <= ((m+1)<<1))
                                 (*q).opacity=(*p).opacity+0;
                              else
                                 (*q).opacity=(*n).opacity+0;
                            }
                        }
                      else /* if (magn_methx == 3 || magn_methx == 5) */
                        {
                          /* Replicate nearest */
                          if (i <= ((m+1)<<1))
                             *q=(*p);
                          else
                             *q=(*n);
                          if (magn_methx == 5)
                            {
                              /* Interpolate */
                              (*q).opacity=(Quantum) ((2*i*((*n).opacity
                                 -(*p).opacity)+m) /((long) (m*2))+(*p).opacity);
                            }
                        }
                      q++;
                    }
                    n++;
                    p++;
                  }
                  if (!SyncImagePixels(image))
                    break;
                }
              }
          }

        /*
          Crop_box is with respect to the upper left corner of the MNG.
        */
        crop_box.left=image_box.left+mng_info->x_off[object_id];
        crop_box.right=image_box.right+mng_info->x_off[object_id];
        crop_box.top=image_box.top+mng_info->y_off[object_id];
        crop_box.bottom=image_box.bottom+mng_info->y_off[object_id];
        crop_box=mng_minimum_box(crop_box,clip);
        crop_box=mng_minimum_box(crop_box,frame);
        crop_box=mng_minimum_box(crop_box,mng_info->object_clip[object_id]);
        if ((crop_box.left != (image_box.left+mng_info->x_off[object_id])) ||
            (crop_box.right != (image_box.right+mng_info->x_off[object_id])) ||
            (crop_box.top != (image_box.top+mng_info->y_off[object_id])) ||
            (crop_box.bottom != (image_box.bottom+mng_info->y_off[object_id])))
          {
            if ((crop_box.left < crop_box.right) &&
                (crop_box.top < crop_box.bottom))
              {
                Image
                  *p;

                RectangleInfo
                  crop_info;

                /*
                  Crop_info is with respect to the upper left corner of
                  the image.
                */
                crop_info.x=(crop_box.left-mng_info->x_off[object_id]);
                crop_info.y=(crop_box.top-mng_info->y_off[object_id]);
                crop_info.width=(crop_box.right-crop_box.left);
                crop_info.height=(crop_box.bottom-crop_box.top);
                image->page.width=image->columns;
                image->page.height=image->rows;
                image->page.x=0;
                image->page.y=0;
                image->orphan=True;
                p=CropImage(image,&crop_info,exception);
                if (p != (Image *) NULL)
                  {
                    p->orphan=True;
                    image->columns=p->columns;
                    image->rows=p->rows;
                    p->orphan=True;
                    p->file=(FILE *) NULL;
                    p->blob->mapped=False;
                    DestroyImage(p);
                    image->page.width=image->columns;
                    image->page.height=image->rows;
                    image->page.x=crop_box.left;
                    image->page.y=crop_box.top;
                  }
              }
            else
              {
                /*
                  No pixels in crop area.  The MNG spec still requires
                  a layer, though, so make a single transparent pixel in
                  the top left corner.
                */
                image->columns=1;
                image->rows=1;
                image->colors=2;
                SetImage(image,TransparentOpacity);
                image->page.width=1;
                image->page.height=1;
                image->page.x=0;
                image->page.y=0;
              }
          }
#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
        image=mng_info->image;
#endif
      }

#if (QuantumDepth == 16)
    /* Determine if bit depth can be reduced from 16 to 8.
     * Note that the method GetImageDepth doesn't check background
     * and doesn't handle PseudoClass specially.  Also it uses
     * multiplication and division by 257 instead of shifting, so
     * might be slower.
     */
    if (image->depth == 16)
      {
        int
          ok_to_reduce;

        const PixelPacket
          *p;

        ok_to_reduce=((((image->background_color.red >> 8) & 0xff)
          == (image->background_color.red & 0xff)) &&
           (((image->background_color.green >> 8) & 0xff)
          == (image->background_color.green & 0xff)) &&
           (((image->background_color.blue >> 8) & 0xff)
          == (image->background_color.blue & 0xff)));
        if (ok_to_reduce && image->storage_class == PseudoClass)
          {
            int index;

            for (index=0; index < (long) image->colors; index++)
              {
                ok_to_reduce=((((image->colormap[index].red >> 8) & 0xff)
                  == (image->colormap[index].red & 0xff)) &&
                  (((image->colormap[index].green >> 8) & 0xff)
                  == (image->colormap[index].green & 0xff)) &&
                  (((image->colormap[index].blue >> 8) & 0xff)
                  == (image->colormap[index].blue & 0xff)));
                if (!ok_to_reduce)
                  break;
              }
          }
        if (ok_to_reduce && image->storage_class != PseudoClass)
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              ok_to_reduce=((((p->red >> 8) & 0xff) == (p->red & 0xff)) &&
                (((p->green >> 8) & 0xff) == (p->green & 0xff)) &&
                (((p->blue >> 8) & 0xff) == (p->blue & 0xff)) &&
                (((!image->matte ||
                ((p->opacity >> 8) & 0xff) == (p->opacity & 0xff)))));
              if (!ok_to_reduce)
                break;
              p++;
            }
            if (x < (long) image->columns)
              break;
          }
        if (ok_to_reduce)
          {
            image->depth=8;
#ifdef PNG_DEBUG
            if (image_info->verbose)
              (void) printf("png.c: reducing bit depth to 8 without loss of info\n");
#endif
          }
      }
#endif

      CatchImageException(image);
  } while (LocaleCompare(image_info->magick,"MNG") == 0);
  if (insert_layers && !image_found && (mng_width) && (mng_height))
    {
      /*
        Insert a background layer if nothing else was found.
      */
      if (GetPixels(image) != (PixelPacket *) NULL)
        {
          /*
            Allocate next image structure.
          */
          AllocateNextImage(image_info,image);
          if (image->next == (Image *) NULL)
            {
              DestroyImages(image);
              MngInfoFreeStruct(mng_info,&have_mng_structure);
              return((Image *) NULL);
            }
          image=image->next;
          *image->next->blob=(*image->blob);
        }
      image->columns= mng_width;
      image->rows= mng_height;
      image->page.width=mng_width;
      image->page.height=mng_height;
      image->page.x=0;
      image->page.y=0;
      image->background_color=mng_background_color;
      image->matte=False;
      SetImage(image,OpaqueOpacity);
      image_found++;
    }
  if (ticks_per_second)
     image->delay=(100*final_delay/ticks_per_second);
  else
    {
     image->delay=final_delay;
     image->start_loop=True;
    }
  image->iterations=mng_iterations;
  if (mng_iterations == 1)
    image->start_loop=True;
  while (image->previous != (Image *) NULL)
  {
    image_count++;
    if (image_count>10*image_found)
      {
        ThrowException(&image->exception,(ExceptionType) DelegateWarning,
          "Linked list is corrupted, beginning of list not found",
          image_info->filename);
        return((Image *) NULL);
      }
    image=image->previous;
    if (image->next == (Image *) NULL)
      ThrowException(&image->exception,(ExceptionType) DelegateWarning,
       "Linked list is corrupted; next_image is NULL",image_info->filename);
  }
  if (ticks_per_second && image_found > 1 && image->next == (Image *) NULL)
    ThrowException(&image->exception,(ExceptionType) DelegateWarning,
     "image->next for first image is NULL but shouldn't be.",
     image_info->filename);
  if (!image_found)
    {
      ThrowException(&image->exception,(ExceptionType) DelegateWarning,
        "No visible images in file",image_info->filename);
      if (image != (Image *) NULL)
        DestroyImages(image);
      MngInfoFreeStruct(mng_info,&have_mng_structure);
      return((Image *) NULL);
    }
  if (insert_layers)
    {
      Image
        *next_image,
        *next;

      next_image=CoalesceImages(image,&image->exception);
      if (next_image == (Image *) NULL)
        MagickError(image->exception.severity,image->exception.reason,
          image->exception.description);
      DestroyImages(image);
      image=next_image;

      for(next=image; next->next != (Image *) NULL; next=next_image)
      {
         next_image=next->next;
         if (next->delay == 0)
           {
             if (image == next)
               image=next_image;
             DestroyImage(next);
           }
      }
    }
  CloseBlob(image);
  MngInfoFreeStruct(mng_info,&have_mng_structure);
  have_mng_structure=False;
  return(image);
}
#else
static Image *ReadPNGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,(ExceptionType) MissingDelegateWarning,
    "PNG library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P N G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPNGImage adds attributes for the PNG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPNGImage method is:
%
%      RegisterPNGImage(void)
%
*/
ModuleExport void RegisterPNGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MNG");
  entry->decoder=ReadPNGImage;
  entry->encoder=WritePNGImage;
  entry->magick=IsMNG;
  entry->description=AllocateString("Multiple-image Network Graphics");
  entry->module=AllocateString("PNG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PNG");
  entry->decoder=ReadPNGImage;
  entry->encoder=WritePNGImage;
  entry->magick=IsPNG;
  entry->adjoin=False;
  entry->description=AllocateString("Portable Network Graphics");
  entry->module=AllocateString("PNG");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P N G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPNGImage removes format registrations made by the
%  PNG module from the list of supported formats.
%
%  The format of the UnregisterPNGImage method is:
%
%      UnregisterPNGImage(void)
%
*/
ModuleExport void UnregisterPNGImage(void)
{
  (void) UnregisterMagickInfo("MNG");
  (void) UnregisterMagickInfo("PNG");
}

#if defined(HasPNG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePNGImage writes an image in the Portable Network Graphics
%  encoded image format.
%
%  MNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu
%
%  The format of the WritePNGImage method is:
%
%      unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePNGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
%  To do (as of version 5.2.9, February 14, 2001 -- glennrp -- see also
%    "To do" under ReadPNGImage):
%
%    Fix problem with palette sorting (when PNG_SORT_PALETTE is enabled,
%    some GIF animations don't convert properly)
%
%    Preserve all unknown and not-yet-handled known chunks found in input
%    PNG file and copy them  into output PNG files according to the PNG
%    copying rules.
%
%    Write the iCCP chunk at MNG level when (image->color_profile.length > 0)
%
%    Improve selection of color type (use indexed-colour or indexed-colour
%    with tRNS when 256 or fewer unique RGBA values are present).
%
%    Figure out what to do with "dispose=<restore-to-previous>" (dispose==3)
%    This will be complicated if we limit ourselves to generating MNG-LC
%    files.  For now we ignore disposal method 3 and simply overlay the next
%    image on it.
%
%    Check for identical PLTE's or PLTE/tRNS combinations and use a
%    global MNG PLTE or PLTE/tRNS combination when appropriate.
%    [mostly done 15 June 1999 but still need to take care of tRNS]
%
%    Check for identical sRGB and replace with a global sRGB (and remove
%    gAMA/cHRM if sRGB is found; check for identical gAMA/cHRM and
%    replace with global gAMA/cHRM (or with sRGB if appropriate; replace
%    local gAMA/cHRM with local sRGB if appropriate).
%
%    Check for identical sBIT chunks and write global ones.
%
%    Provide option to skip writing the signature tEXt chunks.
%
%    Use signatures to detect identical objects and reuse the first
%    instance of such objects instead of writing duplicate objects.
%
%    Use a smaller-than-32k value of compression window size when
%    appropriate.
%
%    Encode JNG datastreams.
%
%    Provide an option to force LC files (to ensure exact framing rate)
%    instead of VLC.
%
%    Provide an option to force VLC files instead of LC, even when offsets
%    are present.  This will involve expanding the embedded images with a
%    transparent region at the top and/or left.
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static void PNGLong(png_bytep p,png_uint_32 value)
{
  *p++=(png_byte) ((value >> 24) & 0xff);
  *p++=(png_byte) ((value >> 16) & 0xff);
  *p++=(png_byte) ((value >> 8) & 0xff);
  *p++=(png_byte) (value & 0xff);
}

static void PNGShort(png_bytep p,png_uint_16 value)
{
  *p++=(png_byte) ((value >> 8) & 0xff);
  *p++=(png_byte) (value & 0xff);
}

static void PNGType(png_bytep p,png_bytep type)
{
  (void) memcpy(p,type,4*sizeof(png_byte));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#if (PNG_LIBPNG_VER > 99 && PNG_LIBPNG_VER < 10007)
/* This function became available in libpng version 1.0.6g. */
static void
png_set_compression_buffer_size(png_structp png_ptr, png_uint_32 size)
{
    if(png_ptr->zbuf)
       png_free(png_ptr, png_ptr->zbuf); png_ptr->zbuf=NULL;
    png_ptr->zbuf_size = (png_size_t)size;
    png_ptr->zbuf = (png_bytep)png_malloc(png_ptr, size);
    if(!png_ptr->zbuf)
       png_error(png_ptr,"Unable to allocate zbuf");
}
#endif

static void
png_write_raw_profile(const ImageInfo *image_info,png_struct *ping,
   png_info *ping_info, unsigned char *profile_type, unsigned char
   *profile_description, unsigned char *profile_data, png_uint_32 length)
{
#if (PNG_LIBPNG_VER > 10005)
   png_textp
     text;

   register int
     i;

   unsigned char
     *sp;

   png_charp
     dp;

   png_uint_32
     allocated_length,
     description_length;

   unsigned char
     hex[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
#endif

#if (PNG_LIBPNG_VER <= 10005)
   if (image_info->verbose)
     (void) printf("Not ");
   image_info=image_info;
   ping=ping;
   ping_info=ping_info;
   profile_type=profile_type;
   profile_description=profile_description;
   profile_data=profile_data;
   length=length;
#endif
   if (image_info->verbose)
     {
     (void) printf("writing raw profile: type=%.1024s, length=%lu\n",
       profile_type, length);
     }
#if (PNG_LIBPNG_VER > 10005)
   text=(png_textp) png_malloc(ping,(png_uint_32) sizeof(png_text));
   description_length=strlen((const char *) profile_description);
   allocated_length= (png_uint_32) (length*2 + (length>>5) + 20
      + description_length);
   text[0].text=(png_charp) png_malloc(ping,allocated_length);
   text[0].key=(png_charp) png_malloc(ping, (png_uint_32) 80);
   text[0].key[0]='\0';
   (void) strcat(text[0].key, "Raw profile type ");
   (void) strncat(text[0].key, (const char *) profile_type, 61);
   sp=profile_data;
   dp=text[0].text;
   *dp++='\n';
   (void) strcat(dp,(const char *) profile_description);
   dp+=description_length;
   *dp++='\n';
   (void) sprintf(dp,"%8lu ",length);
   dp+=8;
   for (i=0; i<(long) length; i++)
   {
     if (i%36 == 0)
       *dp++ = '\n';
     *(dp++)=hex[((*sp>>4) & 0x0f)];
     *(dp++)=hex[((*sp++ ) & 0x0f)];
   }
   *dp++='\n';
   *dp='\0';
   text[0].text_length=dp-text[0].text;
   text[0].compression=image_info->compression == NoCompression ||
     (image_info->compression == UndefinedCompression &&
     text[0].text_length < 128) ? -1 : 0;
   assert(text[0].text_length <= allocated_length);
   png_set_text(ping,ping_info,text,1);
   png_free(ping,text[0].text);
   png_free(ping,text[0].key);
   png_free(ping,text);
#endif
}

static unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
{
  Image
    *next_image;

  ImageAttribute
    *attribute;

  int
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
    equal_palettes,
    need_local_plte,
#endif
    all_images_are_gray,
    equal_backgrounds=True,
    equal_chrms,
    equal_gammas,
    equal_physs=True,
    equal_srgbs,
    framing_mode,
    have_write_global_chrm,
    have_write_global_gama,
    have_write_global_plte,
    have_write_global_srgb,
    image_count,
    need_defi,
    need_fram,
    need_iterations,
    need_matte,
    old_framing_mode,
    use_global_plte,
    zero=0;

  unsigned long
    rowbytes;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x=0;

  register const PixelPacket
    *p=NULL;

  register long
    i;

  RectangleInfo
    page;

  png_info
    *ping_info;

  png_struct
    *ping;

  unsigned char
    chunk[800],
    *png_pixels,
    **scanlines;

  unsigned int
    adjoin,
    matte,
    scene,
    status;

  unsigned long
    delay,
    final_delay=0,
    initial_delay,
    save_image_depth,
    ticks_per_second=0;

  /*
    Open image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException((ExceptionType) FileOpenWarning,
      "Unable to open file",image);
  use_global_plte=False;
  page.width=0;
  page.height=0;
  page.x=0;
  page.y=0;
  all_images_are_gray=False;
  have_write_global_plte=False;
#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
  need_local_plte=True;
#endif
  have_write_global_srgb=False;
  have_write_global_gama= False;
  have_write_global_chrm=False;
  need_defi=False;
  need_fram=False;
  need_matte=False;
  framing_mode=1;
  old_framing_mode=1;

  adjoin=image_info->adjoin && (image->next != (Image *) NULL);
  if (adjoin)
    {
      unsigned int
        need_geom;

      unsigned short
        red,
        green,
        blue;

      /*
        Determine image bounding box.
      */
      need_geom=True;
      if (image_info->page != (char *) NULL)
        {
          (void) ParseGeometry(image_info->page,&page.x,&page.y,
            &page.width,&page.height);
          need_geom=False;
        }
      /*
        Check all the scenes.
      */
      initial_delay=image->delay;
      need_iterations=False;
      equal_chrms=image->chromaticity.white_point.x != 0.0;
      equal_gammas=True;
      equal_srgbs=True;
      image_count=0;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
      all_images_are_gray=True;
      equal_palettes=False;
      need_local_plte=False;
#endif
      for (next_image=image; next_image != (Image *) NULL; )
      {
        page=next_image->page;
        if (need_geom)
          {
            if ((next_image->columns+page.x) > page.width)
              page.width=next_image->columns+page.x;
            if ((next_image->rows+page.y) > page.height)
              page.height=next_image->rows+page.y;
          }
        if (page.x || page.y)
          need_defi=True;
        if (next_image->matte)
          need_matte=True;
        if (next_image->dispose >= 2)
          if (next_image->matte || page.x || page.y ||
              ((next_image->columns < page.width) &&
               (next_image->rows < page.height)))
            need_fram=True;
        if (next_image->iterations)
          need_iterations=True;
        final_delay=next_image->delay;
        if (final_delay != initial_delay)
          need_fram=1;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
        /*
          check for global palette possibility.
        */
        if (image->matte)
           need_local_plte=True;
        if (!need_local_plte)
          {
            if (!ImageIsGray(image))
              all_images_are_gray=False;
            equal_palettes=PalettesAreEqual(image,next_image);
            if (!use_global_plte)
              use_global_plte=equal_palettes;
            need_local_plte=!equal_palettes;
          }
#endif
        if (next_image->next != (Image *) NULL)
          {
            if (next_image->background_color.red !=
                next_image->next->background_color.red ||
                next_image->background_color.green !=
                next_image->next->background_color.green ||
                next_image->background_color.blue !=
                next_image->next->background_color.blue)
              equal_backgrounds=False;
            if (next_image->gamma != next_image->next->gamma)
              equal_gammas=False;
            if (next_image->rendering_intent !=
                next_image->next->rendering_intent)
              equal_srgbs=False;
            if ((next_image->units != next_image->next->units) ||
                (next_image->x_resolution != next_image->next->x_resolution) ||
                (next_image->y_resolution != next_image->next->y_resolution))
              equal_physs=False;
            if (equal_chrms)
              {
                if (next_image->chromaticity.red_primary.x !=
                    next_image->next->chromaticity.red_primary.x ||
                    next_image->chromaticity.red_primary.y !=
                    next_image->next->chromaticity.red_primary.y ||
                    next_image->chromaticity.green_primary.y !=
                    next_image->next->chromaticity.green_primary.x ||
                    next_image->chromaticity.blue_primary.x !=
                    next_image->next->chromaticity.green_primary.y ||
                    next_image->chromaticity.blue_primary.x !=
                    next_image->next->chromaticity.blue_primary.x ||
                    next_image->chromaticity.blue_primary.y !=
                    next_image->next->chromaticity.blue_primary.y ||
                    next_image->chromaticity.white_point.x !=
                    next_image->next->chromaticity.white_point.x ||
                    next_image->chromaticity.white_point.y !=
                    next_image->next->chromaticity.white_point.y)
                  equal_chrms=False;
              }
          }
        image_count++;
        next_image=next_image->next;
      }
      if (image_count < 2)
        {
          equal_backgrounds=False;
          equal_chrms=False;
          equal_gammas=False;
          equal_srgbs=False;
          equal_physs=False;
          use_global_plte=False;
#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
          need_local_plte=True;
#endif
          need_iterations=False;
        }
     if (!need_fram)
       {
         /*
           Only certain framing rates 100/n are exactly representable without
           the FRAM chunk but we'll allow some slop in VLC files
         */
         if (final_delay == 0)
           {
             if (need_iterations)
               {
                 /*
                   It's probably a GIF with loop; don't run it *too* fast.
                 */
                 final_delay=10;
                 ThrowException(&image->exception,(ExceptionType)
                   DelegateWarning,
                   "input has zero delay between all frames; assuming 10 cs",
                   (char *) NULL);
               }
             else
               ticks_per_second=0;
           }
         if (final_delay)
           ticks_per_second=100/final_delay;
         if (final_delay > 50)
           ticks_per_second=2;
         if (final_delay > 75)
           ticks_per_second=1;
         if (final_delay > 125)
           need_fram=True;
         if (need_defi && final_delay > 2 && (final_delay != 4) &&
             (final_delay != 5) && (final_delay != 10) && (final_delay != 20) &&
             (final_delay != 25) && (final_delay != 50) && (final_delay != 100))
           need_fram=True;  /* make it exact; we cannot have VLC anyway */
       }
     if (need_fram)
        ticks_per_second = 100;
     /*
        If pseudocolor, we should also check to see if all the
        palettes are identical and write a global PLTE if they are.
        ../glennrp Feb 99.
     */
     /*
        Write the MNG version 0.96 signature and MHDR chunk.
     */
     (void) WriteBlob(image,8,"\212MNG\r\n\032\n");
     (void) WriteBlobMSBLong(image,28L);  /* chunk data length = 28 */
     PNGType(chunk,mng_MHDR);
     PNGLong(chunk+4,page.width);
     PNGLong(chunk+8,page.height);
     PNGLong(chunk+12,ticks_per_second);
     PNGLong(chunk+16,0L);  /* layer count = unknown */
     PNGLong(chunk+20,0L);  /* frame count = unknown */
     PNGLong(chunk+24,0L);  /* play time = unknown   */
     if (need_matte)
       {
         if (need_defi || need_fram || use_global_plte)
           PNGLong(chunk+28,11L);    /* simplicity =  LC */
         else
           PNGLong(chunk+28,9L);    /* simplicity = VLC */
       }
     else
       {
         if (need_defi || need_fram || use_global_plte)
           PNGLong(chunk+28,3L);    /* simplicity =  LC, no transparency */
         else
           PNGLong(chunk+28,1L);    /* simplicity = VLC, no transparency */
       }
     (void) WriteBlob(image,32,(char *) chunk);
     (void) WriteBlobMSBLong(image,crc32(0,chunk,32));
     if ((image->previous == (Image *) NULL) &&
         (image->next != (Image *) NULL) && (image->iterations != 1))
       {
         /*
           Write MNG TERM chunk
         */
         (void) WriteBlobMSBLong(image,10L);  /* data length = 10 */
         PNGType(chunk,mng_TERM);
         chunk[4]=3;  /* repeat animation */
         chunk[5]=0;  /* show last frame when done */
         PNGLong(chunk+6,(png_uint_32) (ticks_per_second*final_delay/100));
         if (image->iterations == 0)
           PNGLong(chunk+10,PNG_MAX_UINT);
         else
           PNGLong(chunk+10,(png_uint_32) image->iterations);
         (void) WriteBlob(image,14,(char *) chunk);
         (void) WriteBlobMSBLong(image,crc32(0,chunk,14));
       }
     /*
       To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
     */
     if ((image_info->colorspace==sRGBColorspace || image->rendering_intent) &&
          equal_srgbs)
       {
         /*
           Write MNG sRGB chunk
         */
         (void) WriteBlobMSBLong(image,1L);
         PNGType(chunk,mng_sRGB);
         chunk[4]=image->rendering_intent+1;
         (void) WriteBlob(image,5,(char *) chunk);
         (void) WriteBlobMSBLong(image,crc32(0,chunk,5));
         have_write_global_srgb=True;
       }
     else
       {
         if (image->gamma && equal_gammas)
           {
             /*
                Write MNG gAMA chunk
             */
             (void) WriteBlobMSBLong(image,4L);
             PNGType(chunk,mng_gAMA);
             PNGLong(chunk+4,(unsigned long) (100000*image->gamma+0.5));
             (void) WriteBlob(image,8,(char *) chunk);
             (void) WriteBlobMSBLong(image,crc32(0,chunk,8));
             have_write_global_gama=True;
           }
         if (equal_chrms)
           {
             /*
                Write MNG cHRM chunk
             */
             (void) WriteBlobMSBLong(image,32L);
             PNGType(chunk,mng_cHRM);
             PNGLong(chunk+4,(unsigned long)
               (100000*image->chromaticity.white_point.x+0.5));
             PNGLong(chunk+8,(unsigned long)
               (100000*image->chromaticity.white_point.y+0.5));
             PNGLong(chunk+12,(unsigned long)
               (100000*image->chromaticity.red_primary.x+0.5));
             PNGLong(chunk+16,(unsigned long)
               (100000*image->chromaticity.red_primary.y+0.5));
             PNGLong(chunk+20,(unsigned long)
               (100000*image->chromaticity.green_primary.x+0.5));
             PNGLong(chunk+24,(unsigned long)
               (100000*image->chromaticity.green_primary.y+0.5));
             PNGLong(chunk+28,(unsigned long)
               (100000*image->chromaticity.blue_primary.x+0.5));
             PNGLong(chunk+32,(unsigned long)
               (100000*image->chromaticity.blue_primary.y+0.5));
             (void) WriteBlob(image,36,(char *) chunk);
             (void) WriteBlobMSBLong(image,crc32(0,chunk,36));
             have_write_global_chrm=True;
           }
       }
     if (image->x_resolution && image->y_resolution && equal_physs)
       {
         /*
            Write MNG pHYs chunk
         */
         (void) WriteBlobMSBLong(image,9L);
         PNGType(chunk,mng_pHYs);
         if (image->units == PixelsPerInchResolution)
           {
             PNGLong(chunk+4,(unsigned long)
               (image->x_resolution*100.0/2.54+0.5));
             PNGLong(chunk+8,(unsigned long)
               (image->y_resolution*100.0/2.54+0.5));
             chunk[12]=1;
           }
         else
           {
             if (image->units==PixelsPerCentimeterResolution)
               {
                 PNGLong(chunk+4,(unsigned long)
                   (image->x_resolution*100.0+0.5));
                 PNGLong(chunk+8,(unsigned long)
                   (image->y_resolution*100.0+0.5));
                 chunk[12]=1;
               }
             else
               {
                 PNGLong(chunk+4,(unsigned long) (image->x_resolution+0.5));
                 PNGLong(chunk+8,(unsigned long) (image->y_resolution+0.5));
                 chunk[12]=0;
               }
           }
         (void) WriteBlob(image,13,(char *) chunk);
         (void) WriteBlobMSBLong(image,crc32(0,chunk,13));
       }
     /*
       Write MNG BACK chunk and global bKGD chunk, if the image is transparent
       or does not cover the entire frame.
     */
     if (image->matte || image->page.x > 0 || image->page.y > 0 ||
         (image->page.width && (image->page.width+image->page.x < page.width)) ||
         (image->page.height && (image->page.height+image->page.y < page.height)))
       {
         (void) WriteBlobMSBLong(image,6L);
         PNGType(chunk,mng_BACK);
         red=UpScale(image->background_color.red);
         green=UpScale(image->background_color.green);
         blue=UpScale(image->background_color.blue);
         PNGShort(chunk+4,red);
         PNGShort(chunk+6,green);
         PNGShort(chunk+8,blue);
         (void) WriteBlob(image,10,(char *) chunk);
         (void) WriteBlobMSBLong(image,crc32(0,chunk,10));
         if (equal_backgrounds)
           {
             (void) WriteBlobMSBLong(image,6L);
             PNGType(chunk,mng_bKGD);
             (void) WriteBlob(image,10,(char *) chunk);
             (void) WriteBlobMSBLong(image,crc32(0,chunk,10));
           }
       }

#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
     if (!need_local_plte && image->storage_class==PseudoClass
         && !all_images_are_gray)
       {
         unsigned long
           data_length;

         /*
           Write MNG PLTE chunk
         */
         data_length=3*image->colors;
         (void) WriteBlobMSBLong(image,data_length);
         PNGType(chunk,mng_PLTE);
         for (i=0; i < (long) image->colors; i++)
         {
           chunk[4+i*3]=DownScale(image->colormap[i].red) & 0xff;
           chunk[5+i*3]=DownScale(image->colormap[i].green) & 0xff;
           chunk[6+i*3]= DownScale(image->colormap[i].blue) & 0xff;
         }
         (void) WriteBlob(image,data_length+4,(char *) chunk);
         (void) WriteBlobMSBLong(image,crc32(0,chunk,(int) (data_length+4)));
         have_write_global_plte=True;
       }
#endif
    }
  scene=0;
  delay=0;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
  equal_palettes=False;
#endif
  do
  {
    png_colorp
       palette;

    int
       not_valid;

#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
    /*
      If we aren't using a global palette for the entire MNG, check to
      see if we can use one for two or more consecutive images.
    */
    if (need_local_plte && use_global_plte && !all_images_are_gray)
      {
        if (image->storage_class==PseudoClass)
          {
            /*
              When equal_palettes is true, this image has the same palette
              as the previous PseudoClass image
            */
            have_write_global_plte=equal_palettes;
            equal_palettes=PalettesAreEqual(image,image->next);
            if (equal_palettes && !have_write_global_plte)
              {
                /*
                  Write MNG PLTE chunk
                */
                unsigned long
                  data_length;

                data_length=3*image->colors;
                (void) WriteBlobMSBLong(image,data_length);
                PNGType(chunk,mng_PLTE);
                for (i=0; i < (long) image->colors; i++)
                {
                  chunk[4+i*3]=DownScale(image->colormap[i].red) & 0xff;
                  chunk[5+i*3]=DownScale(image->colormap[i].green) & 0xff;
                  chunk[6+i*3]=DownScale(image->colormap[i].blue) & 0xff;
                }
                (void) WriteBlob(image,data_length+4,(char *) chunk);
                (void) WriteBlobMSBLong(image,crc32(0,chunk,(int) (data_length+4)));
                have_write_global_plte=True;
              }
          }
        else
          have_write_global_plte=False;
      }
#endif
    if (need_defi)
      {
        long
          previous_x,
          previous_y;

        if (scene)
          {
            previous_x=page.x;
            previous_y=page.y;
          }
        else
          {
            previous_x=0;
            previous_y=0;
          }
        page=image->page;
        if ((page.x !=  previous_x) || (page.y != previous_y))
          {
             (void) WriteBlobMSBLong(image,12L);  /* data length = 12 */
             PNGType(chunk,mng_DEFI);
             chunk[4]=0; /* object 0 MSB */
             chunk[5]=0; /* object 0 LSB */
             chunk[6]=0; /* visible  */
             chunk[7]=0; /* abstract */
             PNGLong(chunk+8,page.x);
             PNGLong(chunk+12,page.y);
             (void) WriteBlob(image,16,(char *) chunk);
             (void) WriteBlobMSBLong(image,crc32(0,chunk,16));
          }
      }
    /*
      Allocate the PNG structures
    */
    (void) TransformRGBImage(image,(ColorspaceType) RGBColorspace);
#ifdef PNG_USER_MEM_SUPPORTED
    ping = png_create_write_struct_2(PNG_LIBPNG_VER_STRING,image,
      PNGErrorHandler,PNGWarningHandler,(void *) NULL,
      (png_malloc_ptr) png_IM_malloc,(png_free_ptr) png_IM_free);
#else
    ping=png_create_write_struct(PNG_LIBPNG_VER_STRING,image,
      PNGErrorHandler,PNGWarningHandler);
#endif
    if (ping == (png_struct *) NULL)
      ThrowWriterException((ExceptionType) ResourceLimitWarning,
        "Memory allocation failed",image);
    ping_info=png_create_info_struct(ping);
    if (ping_info == (png_info *) NULL)
      {
        png_destroy_write_struct(&ping,(png_info **) NULL);
        ThrowWriterException((ExceptionType) ResourceLimitWarning,
           "Memory allocation failed",image)
      }
    png_set_write_fn(ping,image,png_put_data,png_flush_data);
    png_pixels=(unsigned char *) NULL;
    scanlines=(unsigned char **) NULL;
    if (setjmp(ping->jmpbuf))
      {
        /*
          PNG write failed.
        */
#ifdef PNG_DEBUG
       if (image_info->verbose)
          (void) printf("PNG write has failed.\n");
#endif
        png_destroy_write_struct(&ping,&ping_info);
        if (scanlines != (unsigned char **) NULL)
          LiberateMemory((void **) &scanlines);
        if (png_pixels != (unsigned char *) NULL)
          LiberateMemory((void **) &png_pixels);
        CloseBlob(image);
        return(False);
      }
    /*
      Prepare PNG for writing.
    */
#if defined(PNG_MNG_FEATURES_SUPPORTED)
    if (adjoin)
       png_permit_mng_features(ping,PNG_ALL_MNG_FEATURES);
#else
# ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
    if (adjoin)
       png_permit_empty_plte(ping,True);
# endif
#endif
    ping_info->width=image->columns;
    ping_info->height=image->rows;
    save_image_depth=image->depth;
    ping_info->bit_depth=(png_byte) save_image_depth;
    if ((image->x_resolution != 0) && (image->y_resolution != 0) &&
        (!adjoin || !equal_physs))
      {
        int
          unit_type;

        png_uint_32
          x_resolution,
          y_resolution;

        if (image->units == PixelsPerInchResolution)
          {
            unit_type=PNG_RESOLUTION_METER;
            x_resolution=(png_uint_32) (100.0*image->x_resolution/2.54);
            y_resolution=(png_uint_32) (100.0*image->y_resolution/2.54);
          }
        else if (image->units == PixelsPerCentimeterResolution)
          {
            unit_type=PNG_RESOLUTION_METER;
            x_resolution=(png_uint_32) (100.0*image->x_resolution);
            y_resolution=(png_uint_32) (100.0*image->y_resolution);
          }
        else
          {
            unit_type=PNG_RESOLUTION_UNKNOWN;
            x_resolution=(png_uint_32) image->x_resolution;
            y_resolution=(png_uint_32) image->y_resolution;
          }
         png_set_pHYs(ping,ping_info,x_resolution,y_resolution,unit_type);
      }
    if (image->matte && (!adjoin || !equal_backgrounds))
      {
        png_color_16
          background;

        if (image->depth < QuantumDepth)
          {
            background.red=DownScale(image->background_color.red);
            background.green=DownScale(image->background_color.green);
            background.blue=DownScale(image->background_color.blue);
            background.gray=(png_uint_16)
              DownScale(Intensity(image->background_color));
          }
        else
          {
            background.red = image->background_color.red;
            background.green = image->background_color.green;
            background.blue = image->background_color.blue;
            background.gray=(png_uint_16) Intensity(image->background_color);
          }
        background.index=(png_byte) background.gray;
        png_set_bKGD(ping,ping_info,&background);
      }
    /*
      Select the color type.
    */
    if (ImageIsMonochrome(image))
      {
        if (!image->matte)
          ping_info->bit_depth=1;
      }
    ping_info->color_type=PNG_COLOR_TYPE_RGB;
    matte=image->matte;
    if (matte)
      {
        ping_info->color_type=PNG_COLOR_TYPE_GRAY_ALPHA;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            if (!IsGray(*p))
              {
                ping_info->color_type=PNG_COLOR_TYPE_RGB_ALPHA;
                break;
              }
            p++;
          }
        }
        /*
          Determine if there is any transparent color.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            if (p->opacity != OpaqueOpacity)
              break;
            p++;
          }
          if (x < (long) image->columns)
            break;
        }
        if ((y == (long) image->rows) && (x == (long) image->columns))
          {
            /*
              No transparent pixels are present.  Change 4 or 6 to 0 or 2,
              and do not set the PNG_INFO_tRNS flag in ping_info->valid.
            */
            image->matte=False;
            ping_info->color_type&=0x03;
          }
        else
          {
            ping_info->valid|=PNG_INFO_tRNS;
            ping_info->trans_values.red=p->red;
            ping_info->trans_values.green=p->green;
            ping_info->trans_values.blue=p->blue;
            ping_info->trans_values.gray=(png_uint_16) Intensity(*p);
            ping_info->trans_values.index=(unsigned char)
               (MaxRGB-DownScale(p->opacity));
          }
        if (ping_info->valid & PNG_INFO_tRNS)
          {
            /*
              Determine if there is one and only one transparent color
              and if so if it is fully transparent.
            */
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
              x=0;
              if (p == (const PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
              {
                if (p->opacity != OpaqueOpacity)
                  {
                    if (!ColorMatch(ping_info->trans_values,*p,zero))
                    {
                       break;  /* Can't use RGB + tRNS for multiple transparent
                                  colors.  */
                    }
                    if (p->opacity != TransparentOpacity)
                    {
                       break;  /* Can't use RGB + tRNS for semitransparency. */
                    }
                  }
                 else
                  {
                    if (ColorMatch(ping_info->trans_values,*p,zero))
                        break; /* Can't use RGB + tRNS when another pixel
                                  having the same RGB samples is transparent. */
                  }
              p++;
              }
              if (x < (long) image->columns)
                 break;
            }
            if (x < (long) image->columns)
              ping_info->valid&=(~PNG_INFO_tRNS);
          }
        if (ping_info->valid & PNG_INFO_tRNS)
          {
            ping_info->color_type &= 0x03;  /* changes 4 or 6 to 0 or 2 */
            if (image->depth == 8)
              {
                ping_info->trans_values.red&=0xff;
                ping_info->trans_values.green&=0xff;
                ping_info->trans_values.blue&=0xff;
                ping_info->trans_values.gray&=0xff;
              }
          }
      }
    matte=image->matte;
    if (ping_info->valid & PNG_INFO_tRNS)
      image->matte=False;
    if (ImageIsGray(image) && (!image->matte || image->depth >= 8))
      {
        if (image->matte)
            ping_info->color_type=PNG_COLOR_TYPE_GRAY_ALPHA;
        else
          {
            ping_info->color_type=PNG_COLOR_TYPE_GRAY;
            if (save_image_depth == 16 && image->depth == 8)
              ping_info->trans_values.gray*=0x0101;
          }
        if (image->depth > QuantumDepth)
          image->depth=QuantumDepth;
        if (image->colors == 0 || image->colors > MaxRGB+1)
          image->colors = 1<<image->depth;
        if (image->depth > 8)
          ping_info->bit_depth=16;
        else
          {
            if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
              {
                ping_info->bit_depth=1;
                while ((int) (1 << ping_info->bit_depth) < (long) image->colors)
                  ping_info->bit_depth<<=1;
              }
            else if (ping_info->color_type == PNG_COLOR_TYPE_GRAY &&
                image->colors < 17 && image->storage_class==PseudoClass)
              {

              /* Check if grayscale is reducible */
                int
                  i,
                  depth_4_ok=True,
                  depth_2_ok=True,
                  depth_1_ok=True;

                for (i=0; i<(long) image->colors; i++)
                {
                   int
                     intensity=DownScale(image->colormap[i].red);

                   if ((intensity & 0x0f) != ((intensity & 0xf0)>>4))
                     depth_4_ok=depth_2_ok=depth_1_ok=False;
                   else if ((intensity & 0x03) != ((intensity & 0x0c)>>2))
                     depth_2_ok=depth_1_ok=False;
                   else if ((intensity & 0x01) != ((intensity & 0x02)>>1))
                     depth_1_ok=False;
                }
                if(depth_1_ok)
                   ping_info->bit_depth=1;
                else if (depth_2_ok)
                   ping_info->bit_depth=2;
                else if (depth_4_ok)
                   ping_info->bit_depth=4;
              }
          }
      }
    else
      if (image->storage_class==PseudoClass)
      {
        if (image->depth <= 8)
          {
            unsigned long
               number_colors;

            number_colors=image->colors;
            if (matte)
               ping_info->valid|=PNG_INFO_tRNS;
            /*
              Set image palette.
            */
            ping_info->color_type=PNG_COLOR_TYPE_PALETTE;
            ping_info->valid|=PNG_INFO_PLTE;
            if (have_write_global_plte && !matte)
              png_set_PLTE(ping,ping_info,NULL,0);
            else
              {
#ifdef PNG_SORT_PALETTE
                unsigned long
                   save_number_colors;

                save_number_colors=image->colors;
                (void) CompressColormapTransFirst(image);
                number_colors=image->colors;
                image->colors=save_number_colors;
#endif
                palette=(png_color *)
                  AcquireMemory(number_colors*sizeof(png_color));
                if (palette == (png_color *) NULL)
                  ThrowWriterException((ExceptionType) ResourceLimitWarning,
                    "Memory allocation failed",image);
                for (i=0; i < (long) number_colors; i++)
                {
                  palette[i].red=DownScale(image->colormap[i].red);
                  palette[i].green=DownScale(image->colormap[i].green);
                  palette[i].blue=DownScale(image->colormap[i].blue);
                }
                png_set_PLTE(ping,ping_info,palette,(int) number_colors);
              }
            ping_info->bit_depth=1;
            while ((1UL << ping_info->bit_depth) < number_colors)
              ping_info->bit_depth<<=1;
            /*
              Identify which colormap entry is transparent.
            */
            ping_info->trans=(unsigned char *) AcquireMemory(number_colors);
            if (ping_info->trans == (unsigned char *) NULL)
              ThrowWriterException((ExceptionType) ResourceLimitWarning,
                "Memory allocation failed",image);
            assert(number_colors <= 256);
            for (i=0; i < (long) number_colors; i++)
               ping_info->trans[i]=255;
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=0; x < (long) image->columns; x++)
              {
                if (p->opacity != OpaqueOpacity)
                  {
                    IndexPacket
                      index;

                    index=indexes[x];
                    assert(index < number_colors);
                    ping_info->trans[index]=(png_byte) (255-
#if (QuantumDepth == 8)
                       p->opacity);
#else
                       ((p->opacity>>8)&0xff));
#endif
                  }
                p++;
              }
            }
            ping_info->num_trans=0;
            for (i=0; i < (long) number_colors; i++)
              if (ping_info->trans[i] != 255)
                ping_info->num_trans=(unsigned short) (i+1);
            if (ping_info->num_trans == 0)
              ping_info->valid&=(~PNG_INFO_tRNS);
            if (!(ping_info->valid & PNG_INFO_tRNS))
              ping_info->num_trans=0;

            /*
              Identify which colormap entry is the background color.
            */
            for (i=0; i < (long) Max(number_colors-1,1); i++)
              if (ColorMatch(ping_info->background,image->colormap[i],zero))
                break;
            ping_info->background.index=(Quantum) i;
          }
      }
    else
      {
        if (image->depth < 8)
          image->depth=8;
        if ((save_image_depth == 16) && (image->depth == 8))
          {
            ping_info->trans_values.red*=0x0101;
            ping_info->trans_values.green*=0x0101;
            ping_info->trans_values.blue*=0x0101;
            ping_info->trans_values.gray*=0x0101;
          }
      }

    /*
      Initialize compression level and filtering.
    */
#if (PNG_LIBPNG_VER > 99)
    png_set_compression_buffer_size(ping,32768L);
#endif
    png_set_compression_mem_level(ping, 9);
    if (image_info->quality > 9)
       png_set_compression_level(ping,(int) Min(image_info->quality/10,9));
    else
       png_set_compression_strategy(ping, Z_HUFFMAN_ONLY);
#if defined(PNG_MNG_FEATURES_SUPPORTED) && defined(PNG_INTRAPIXEL_DIFFERENCING)
    /* This became available in libpng-1.0.9.  Output must be a MNG. */
    if (adjoin && ((image_info->quality % 10) == 7))
    {
      ping_info->filter_type=PNG_INTRAPIXEL_DIFFERENCING;
    }
#endif
    if ((image_info->quality % 10) > 5)
      png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);
    else
      if ((image_info->quality % 10) != 5)
        png_set_filter(ping,PNG_FILTER_TYPE_BASE,(int) image_info->quality % 10);
      else
        if ((ping_info->color_type == PNG_COLOR_TYPE_GRAY) ||
            (ping_info->color_type == PNG_COLOR_TYPE_PALETTE) ||
            (image_info->quality < 50))
          png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_NO_FILTERS);
        else
          png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);

    if (image->color_profile.length)
#if (PNG_LIBPNG_VER > 10008) && defined(PNG_WRITE_iCCP_SUPPORTED)
      {
        if (image->color_profile.name == (png_charp) NULL)
          png_set_iCCP(ping,ping_info,(png_charp) "icm",
             (int) 0, (png_charp) image->color_profile.info,
             (png_uint_32) image->color_profile.length);
        else
          png_set_iCCP(ping,ping_info,(png_charp) image->color_profile.name,
             (int) 0, (png_charp) image->color_profile.info,
             (png_uint_32) image->color_profile.length);
      }
#else
      png_write_raw_profile(image_info,ping,ping_info,
        (unsigned char *) "icm",
        (unsigned char *) "ICC Profile",
        (unsigned char *) image->color_profile.info,
        (png_uint_32) image->color_profile.length);
#endif
    if (image->iptc_profile.length != 0)
      png_write_raw_profile(image_info,ping,ping_info,
        (unsigned char *) "iptc",
        (unsigned char *) "IPTC profile",
        (unsigned char *) image->iptc_profile.info,
        (png_uint_32) image->iptc_profile.length);
    for (i=0; i < (long) image->generic_profiles; i++)
    {
      if(image->generic_profile[i].name == (png_charp) NULL)
        png_write_raw_profile(image_info,ping,ping_info,
          (unsigned char *) "generic",
          (unsigned char *) "generic profile",
          (unsigned char *) image->generic_profile[i].info,
          (png_uint_32) image->generic_profile[i].length);
      else
        png_write_raw_profile(image_info,ping,ping_info,
          (unsigned char *) image->generic_profile[i].name,
          (unsigned char *) "generic profile",
          (unsigned char *) image->generic_profile[i].info,
          (png_uint_32) image->generic_profile[i].length);
    }

#if defined(PNG_WRITE_sRGB_SUPPORTED)
    if (!have_write_global_srgb &&
        ((image->rendering_intent != UndefinedIntent) ||
        image_info->colorspace == sRGBColorspace))
      {
        /*
          Note image rendering intent.
        */
        png_set_sRGB(ping,ping_info,(int) (image->rendering_intent+1));
        png_set_gAMA(ping,ping_info,0.45455);
      }
    not_valid=(!ping_info->valid);
    if ((!adjoin) || not_valid & PNG_INFO_sRGB)
#endif
      {
        if (!have_write_global_gama && (image->gamma != 0.0))
          {
            /*
              Note image gamma.
              To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
            */
            png_set_gAMA(ping,ping_info,image->gamma);
          }
        if (!have_write_global_chrm &&
            (image->chromaticity.white_point.x != 0.0))
          {
            /*
              Note image chromaticity.
              To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
            */
            png_set_cHRM(ping,ping_info,image->chromaticity.white_point.x,
              image->chromaticity.white_point.y,
              image->chromaticity.red_primary.x,
              image->chromaticity.red_primary.y,
              image->chromaticity.green_primary.x,
              image->chromaticity.green_primary.y,
              image->chromaticity.blue_primary.x,
              image->chromaticity.blue_primary.y);
         }
      }
    ping_info->interlace_type=image_info->interlace != NoInterlace;

    if (need_fram && adjoin && ((image->delay != delay) ||
        (framing_mode != old_framing_mode)))
      {
        if (image->delay == delay)
          {
            /*
              Write a MNG FRAM chunk with the new framing mode.
            */
            (void) WriteBlobMSBLong(image,1L);  /* data length = 1 */
            PNGType(chunk,mng_FRAM);
            chunk[4]=(unsigned char) framing_mode;
            (void) WriteBlob(image,5,(char *) chunk);
            (void) WriteBlobMSBLong(image,crc32(0,chunk,5));
          }
        else
          {
            /*
              Write a MNG FRAM chunk with the delay.
            */
            (void) WriteBlobMSBLong(image,10L);  /* data length = 10 */
            PNGType(chunk,mng_FRAM);
            chunk[4]=(unsigned char) framing_mode;
            chunk[5]=0;  /* frame name separator (no name) */
            chunk[6]=2;  /* flag for changing default delay */
            chunk[7]=0;  /* flag for changing frame timeout */
            chunk[8]=0;  /* flag for changing frame clipping */
            chunk[9]=0;  /* flag for changing frame sync_id */
            PNGLong(chunk+10,(png_uint_32)
              ((ticks_per_second*image->delay)/100));
            (void) WriteBlob(image,14,(char *) chunk);
            (void) WriteBlobMSBLong(image,crc32(0,chunk,14));
            delay=image->delay;
          }
        old_framing_mode=framing_mode;
      }

    if (adjoin)
      png_set_sig_bytes(ping,8);

    png_write_info(ping,ping_info);
    png_set_packing(ping);
    /*
      Allocate memory.
    */
    rowbytes=image->columns;
    if (image->depth == 8)
      {
        if (ImageIsGray(image))
          rowbytes*=(image->matte ? 2 : 1);
        else
          {
            if (image->storage_class!=PseudoClass)
              rowbytes*=(image->matte ? 4 : 3);
          }
      }
    else
      if (image->depth == 16)
        {
          if (ImageIsGray(image))
            rowbytes*=(image->matte ? 4 : 2);
          else
            rowbytes*=(image->matte ? 8 : 6);
        }
    png_pixels=(unsigned char *)
      AcquireMemory(rowbytes*image->rows*sizeof(Quantum));
    scanlines=(unsigned char **)
      AcquireMemory(image->rows*sizeof(unsigned char *));
    if ((png_pixels == (unsigned char *) NULL) ||
        (scanlines == (unsigned char **) NULL))
      ThrowWriterException((ExceptionType) ResourceLimitWarning,
        "Memory allocation failed",image);
    /*
      Initialize image scanlines.
    */
    for (i=0; i < (long) image->rows; i++)
      scanlines[i]=png_pixels+(rowbytes*i);
    if (!image->matte && ImageIsMonochrome(image))
      {
        /*
          Convert PseudoClass image to a PNG monochrome image.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          if (!AcquireImagePixels(image,0,y,image->columns,1,&image->exception))
            break;
          if (image->storage_class == PseudoClass)
                (void) PopImagePixels(image,(QuantumType) GrayQuantum,
                  scanlines[y]);
          else
            (void) PopImagePixels(image,(QuantumType) RedQuantum,scanlines[y]);
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
      }
    else
      {
      if ((!image->matte || (ping_info->bit_depth >= QuantumDepth))
          && ImageIsGray(image))
        {
          for (y=0; y < (long) image->rows; y++)
          {
            if (!AcquireImagePixels(image,0,y,image->columns,1,&image->exception))
              break;
            if (ping_info->color_type == PNG_COLOR_TYPE_GRAY)
              {
                if (image->storage_class == PseudoClass)
                  (void) PopImagePixels(image,(QuantumType) GrayQuantum,
                     scanlines[y]);
                else
                  (void) PopImagePixels(image,(QuantumType) RedQuantum,
                     scanlines[y]);
              }
            else /* PNG_COLOR_TYPE_GRAY_ALPHA */
              {
                if (image->storage_class == DirectClass)
                  {
                    unsigned char
                       *p,
                       *r;

                    (void) PopImagePixels(image,(QuantumType) RGBAQuantum,
                       scanlines[y]);
                    p=scanlines[y];
                    r=p;
                    if (image->depth == 8)
                      for (x=0; x < (long) image->columns; x++)
                        {
                          p++;       /* skip red sample */
                          *r++=*p++; /* copy green sample */
                          p++;       /* skip blue sample */
                          *r++=*p++; /* copy alpha sample */
                        }
#if (QuantumDepth == 16)
                    else /* image->depth == 16 */
                      for (x=0; x < (long) image->columns; x++)
                        {
                          p+=2;       /* skip red sample */
                          *r++=*p++;  /* copy green sample */
                          *r++=*p++;
                          p+=2;       /* skip blue sample */
                          *r++=*p++;  /* copy alpha sample */
                          *r++=*p++;
                        }
#endif
                  }
                else /* PseudoClass */
                   (void) PopImagePixels(image,(QuantumType) GrayAlphaQuantum,
                     scanlines[y]);
              }
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                MagickMonitor(SaveImageText,y,image->rows);
          }
        }
      else
        {
          if ((image->depth > 8) || image->storage_class!=PseudoClass)
            for (y=0; y < (long) image->rows; y++)
            {
              if (!AcquireImagePixels(image,0,y,image->columns,1,&image->exception))
                break;
              if (ping_info->color_type == PNG_COLOR_TYPE_GRAY)
                {
                  if (image->storage_class == DirectClass)
                    (void) PopImagePixels(image,(QuantumType) RedQuantum,
                       scanlines[y]);
                  else
                    (void) PopImagePixels(image,(QuantumType) GrayQuantum,
                      scanlines[y]);
                }
              else if (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                (void) PopImagePixels(image,(QuantumType) GrayAlphaQuantum,
                   scanlines[y]);
              else if (image->matte)
                (void) PopImagePixels(image,(QuantumType) RGBAQuantum,
                  scanlines[y]);
              else
                (void) PopImagePixels(image,(QuantumType) RGBQuantum,
                  scanlines[y]);
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
        else
          for (y=0; y < (long) image->rows; y++)
          {
            if (!AcquireImagePixels(image,0,y,image->columns,1,&image->exception))
              break;
            if (ping_info->color_type == PNG_COLOR_TYPE_GRAY)
              (void) PopImagePixels(image,(QuantumType) GrayQuantum,
                scanlines[y]);
            else
              (void) PopImagePixels(image,(QuantumType) IndexQuantum,
                scanlines[y]);
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                MagickMonitor(SaveImageText,y,image->rows);
          }
       }
    }
    png_write_image(ping,scanlines);
    /*
      Generate text chunks.
    */
#if (PNG_LIBPNG_VER <= 10005)
    ping_info->num_text=0;
#endif
    attribute=GetImageAttribute(image,(char *) NULL);
    for ( ; attribute != (ImageAttribute *) NULL; attribute=attribute->next)
    {
#if (PNG_LIBPNG_VER > 10005)
      png_textp
        text;
#endif

      if (*attribute->key == '[')
        continue;
#if (PNG_LIBPNG_VER > 10005)
      text=(png_textp) png_malloc(ping,(png_uint_32) sizeof(png_text));
      text[0].key=attribute->key;
      text[0].text=attribute->value;
      text[0].text_length=strlen(attribute->value);
      text[0].compression=image_info->compression == NoCompression ||
        (image_info->compression == UndefinedCompression &&
        text[0].text_length < 128) ? -1 : 0;
      png_set_text(ping,ping_info,text,1);
      png_free(ping,text);
#else
  /* Work directly with ping_info struct; png_set_text before libpng version
   * 1.0.5a is leaky */
      if (ping_info->num_text == 0)
        {
          ping_info->text=(png_text *) AcquireMemory(256*sizeof(png_text));
          if (ping_info->text == (png_text *) NULL)
            ThrowException(&image->exception,(ExceptionType)
              ResourceLimitWarning,"Memory allocation failed",image->filename);
        }
      i=ping_info->num_text++;
      if (i > 255)
        ThrowException(&image->exception,(ExceptionType) ResourceLimitWarning,
          "Cannot write more than 256 PNG text chunks",image->filename);
      ping_info->text[i].key=attribute->key;
      ping_info->text[i].text=attribute->value;
      ping_info->text[i].text_length=strlen(attribute->value);
      ping_info->text[i].compression=image_info->compression == NoCompression ||
        (image_info->compression == UndefinedCompression &&
        ping_info->text[i].text_length < 128) ? -1 : 0;
#endif
    }
    png_write_end(ping,ping_info);
    if (need_fram && image->dispose == 2)
      {
        if (page.x || page.y || (ping_info->width != page.width) ||
            (ping_info->height != page.height))
          {
            /*
              Write FRAM 4 with clipping boundaries followed by FRAM 1.
            */
            (void) WriteBlobMSBLong(image,27L);  /* data length = 27 */
            PNGType(chunk,mng_FRAM);
            chunk[4]=4;
            chunk[5]=0;  /* frame name separator (no name) */
            chunk[6]=1;  /* flag for changing delay, for next frame only */
            chunk[7]=0;  /* flag for changing frame timeout */
            chunk[8]=1;  /* flag for changing frame clipping for next frame */
            chunk[9]=0;  /* flag for changing frame sync_id */
            PNGLong(chunk+10,(png_uint_32) (0L)); /* temporary 0 delay */
            chunk[14]=0; /* clipping boundaries delta type */
            PNGLong(chunk+15,(png_uint_32) (page.x)); /* left cb */
            PNGLong(chunk+19,(png_uint_32) (page.x + ping_info->width));
            PNGLong(chunk+23,(png_uint_32) (page.y)); /* top cb */
            PNGLong(chunk+27,(png_uint_32) (page.y + ping_info->height));
            (void) WriteBlob(image,31,(char *) chunk);
            (void) WriteBlobMSBLong(image,crc32(0,chunk,31));
            old_framing_mode=4;
            framing_mode=1;
          }
        else
          framing_mode=3;
      }
    if (need_fram && (image->dispose == 3))
       ThrowException(&image->exception,(ExceptionType) DelegateWarning,
         "Cannot convert GIF with disposal method 3 to MNG-LC",(char *) NULL);
    image->depth=save_image_depth;
    /*
      Free PNG resources.
    */
    if (ping_info->valid & PNG_INFO_PLTE)
      {
        LiberateMemory((void **) &ping_info->palette);
        ping_info->valid&=(~PNG_INFO_PLTE);
      }
    png_destroy_write_struct(&ping,&ping_info);
    LiberateMemory((void **) &scanlines);
    LiberateMemory((void **) &png_pixels);
    CatchImageException(image);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (adjoin);
  if (adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  if (adjoin)
    {
      /*
        Write the MEND chunk.
      */
      (void) WriteBlobMSBLong(image,0x00000000L);
      PNGType(chunk,mng_MEND);
      (void) WriteBlob(image,4,(char *) chunk);
      (void) WriteBlobMSBLong(image,crc32(0,chunk,4));
    }
  /*
    Free memory.
  */
  CloseBlob(image);
  return(True);
}
#else
static unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException((ExceptionType) MissingDelegateWarning,
    "PNG library is not available", image->filename);
}
#endif
