/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                   EEEEE  FFFFF  FFFFF  EEEEE  CCCC  TTTTT                   %
%                   E      F      F      E     C        T                     %
%                   EEE    FFF    FFF    EEE   C        T                     %
%                   E      F      F      E     C        T                     %
%                   EEEEE  F      F      EEEEE  CCCC    T                     %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Image Effects Methods                     %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 October 1996                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/color.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/fx.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/operator.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_iterator.h"
#include "magick/render.h"
#include "magick/shear.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A d a p t i v e T h r e s h o l d I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AdaptiveThresholdImage() selects an individual threshold for each pixel
%  based on the range of intensity values in its local neighborhood.  This
%  allows for thresholding of an image whose global intensity histogram
%  doesn't contain distinctive peaks.
%
%  The format of the AdaptiveThresholdImage method is:
%
%      Image *AdaptiveThresholdImage(Image *image,const unsigned long width,
%        const unsigned long height,const unsigned long unsigned long,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o width: The width of the local neighborhood.
%
%    o height: The height of the local neighborhood.
%
%    o offset: The mean offset.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AdaptiveThresholdImageText  "Adaptive threshold the image...  "
MagickExport Image *AdaptiveThresholdImage(const Image *image,
                                           const unsigned long width,
                                           const unsigned long height,
                                           const double offset,
                                           ExceptionInfo *exception)
{

  Image
    *threshold_image;

  long
    y;

  volatile MagickPassFail
    status;

  /*
    Initialize thresholded image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < width) || (image->rows < height))
    ThrowImageException3(OptionError,UnableToThresholdImage,
                         ImageSmallerThanRadius);
  threshold_image=CloneImage(image,0,0,True,exception);
  if (threshold_image == (Image *) NULL)
    return((Image *) NULL);
  if (image->is_monochrome)
    return threshold_image;
  (void) SetImageType(threshold_image,TrueColorType);
  status=MagickPass;
  /*
    Adaptive threshold image.
  */
  {
    unsigned long
      row_count=0;

    ThreadViewSet
      *read_view_set,
      *write_view_set;

    DoublePixelPacket
      zero;

    read_view_set=AllocateThreadViewSet((Image *) image,exception);
    write_view_set=AllocateThreadViewSet(threshold_image,exception);
    if ((read_view_set == (ThreadViewSet *) NULL) ||
        (write_view_set == (ThreadViewSet *) NULL))
      {
        DestroyThreadViewSet(read_view_set);
        DestroyThreadViewSet(write_view_set);
        DestroyImage(threshold_image);
        return (Image *) NULL;
      }

    (void) memset(&zero,0,sizeof(DoublePixelPacket));
#pragma omp parallel for schedule(static,64)
    for (y=0; y < (long) image->rows; y++)
      {
        ViewInfo
          *read_view,
          *write_view;

        const PixelPacket
          *p;

        PixelPacket
          *q;

        long
          x;

        MagickBool
          thread_status;

        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        read_view=AccessThreadView(read_view_set);
        p=AcquireCacheView(read_view,-(long) width/2,y-height/2,image->columns+width,
                           height,exception);
        write_view=AccessThreadView(write_view_set);
        q=SetCacheView(write_view,0,y,threshold_image->columns,1);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          thread_status=MagickFail;

        if (thread_status != MagickFail)
          {
            for (x=0; x < (long) image->columns; x++)
              {
                DoublePixelPacket
                  pixel;

                const PixelPacket
                  *r;

                long
                  u,
                  v;

                r=p;
                pixel=zero;
                for (v=0; v < (long) height; v++)
                  {
                    for (u=0; u < (long) width; u++)
                      {
                        pixel.red+=r[u].red;
                        pixel.green+=r[u].green;
                        pixel.blue+=r[u].blue;
                        pixel.opacity+=r[u].opacity;
                      }
                    r+=image->columns+width;
                  }

                pixel.red=pixel.red/(width*height)+offset;
                pixel.green=pixel.green/(width*height)+offset;
                pixel.blue=pixel.blue/(width*height)+offset;
                pixel.opacity=pixel.opacity/(width*height)+offset;

                q->red=q->red <= pixel.red ? 0 : MaxRGB;
                q->green=q->green <= pixel.green ? 0 : MaxRGB;
                q->blue=q->blue <= pixel.blue ? 0 : MaxRGB;
                q->opacity=q->opacity <= pixel.opacity ? 0 : MaxRGB;
                p++;
                q++;
              }
            if (!SyncCacheView(write_view))
              {
                thread_status=MagickFail;
                CopyException(exception,&threshold_image->exception);
              }
          }
#pragma omp critical
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitor(AdaptiveThresholdImageText,row_count,image->rows,exception))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
    DestroyThreadViewSet(write_view_set);
    DestroyThreadViewSet(read_view_set);
  }
  if (MagickFail == status)
    {
      DestroyImage(threshold_image);
      threshold_image=(Image *) NULL;
    }
  else
    {
      threshold_image->is_monochrome=True;
      threshold_image->is_grayscale=True;
    }
  return(threshold_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A d d N o i s e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AddNoiseImage() adds random noise to the image.
%
%  The format of the AddNoiseImage method is:
%
%      Image *AddNoiseImage(const Image *image,const NoiseType noise_type,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o noise_type:  The type of noise: Uniform, Gaussian, Multiplicative,
%      Impulse, Laplacian, or Poisson.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static inline Quantum
GenerateQuantumNoise(const Quantum quantum,const NoiseType noise_type,
                     const double factor,unsigned int *seed)
{
  double
    value;

  value = (double) quantum+
    factor*GenerateDifferentialNoise((double) quantum,noise_type,seed);
  return RoundDoubleToQuantum(value);
}
static MagickPassFail
AddNoiseImagePixels(void *mutable_data,                /* User provided mutable data */
                    const void *immutable_data,        /* User provided immutable data */
                    const Image *source_image,         /* Source image */
                    const PixelPacket *source_pixels,  /* Pixel row in source image */
                    const IndexPacket *source_indexes, /* Pixel row indexes in source image */
                    Image *new_image,                  /* New image */
                    PixelPacket *new_pixels,           /* Pixel row in new image */
                    IndexPacket *new_indexes,          /* Pixel row indexes in new image */
                    const long npixels,                /* Number of pixels in row */
                    ExceptionInfo *exception           /* Exception report */
                    )
{
  /*
    Add noise to image pixle.
  */
  const NoiseType
    noise_type = *((const NoiseType *) immutable_data);

  unsigned int
    seed;

  double
    factor;

  register long
    i;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(source_indexes);
  ARG_NOT_USED(new_image);
  ARG_NOT_USED(new_indexes);
  ARG_NOT_USED(exception);

  seed=MagickRandNewSeed();
  factor=1.0;
  if (source_image->is_grayscale)
    {
      /*
        Intensity noise
      */
      for (i=0; i < npixels; i++)
        {
          new_pixels[i].red=new_pixels[i].green=new_pixels[i].blue=
            GenerateQuantumNoise(PixelIntensityToQuantum(&source_pixels[i]),noise_type,factor,&seed);
          if (source_image->matte && new_image->matte)
            new_pixels[i].opacity=source_pixels[i].opacity;
          else
            new_pixels[i].opacity=OpaqueOpacity;
        }
    }
  else
    {
      /*
        Noise across RGB channels
      */
      for (i=0; i < npixels; i++)
        {
          new_pixels[i].red=GenerateQuantumNoise(source_pixels[i].red,noise_type,factor,&seed);
          new_pixels[i].green=GenerateQuantumNoise(source_pixels[i].green,noise_type,factor,&seed);
          new_pixels[i].blue=GenerateQuantumNoise(source_pixels[i].blue,noise_type,factor,&seed);
          if (source_image->matte && new_image->matte)
            new_pixels[i].opacity=source_pixels[i].opacity;
          else
            new_pixels[i].opacity=OpaqueOpacity;
        }
    }

  return MagickPass;
}

#define AddNoiseImageText  "Add noise to the image...  "
MagickExport Image *AddNoiseImage(const Image *image,const NoiseType noise_type,
  ExceptionInfo *exception)
{
  NoiseType noise = noise_type;

  Image
    *noise_image;

  unsigned int
   is_grayscale;

  /*
    Initialize noise image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  noise_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  is_grayscale=IsGrayImage(image,exception);
  noise_image->storage_class=DirectClass;
  /*
    Add noise in each row.
  */
  (void) PixelIterateDualNew(AddNoiseImagePixels,NULL,
                             AddNoiseImageText,NULL,&noise,
                             image->columns,image->rows,image,0,0,
                             noise_image,0,0,&noise_image->exception);
  noise_image->is_grayscale=is_grayscale;
  return(noise_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     B l a c k T h r e s h o l d I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlackThresholdImage() adjusts the levels of image channels such that
%  values below a specified threshold are set to the minimum value (black)
%  while the remaining pixels are unchanged.
%
%  The format of the BlackThresholdImage method is:
%
%      MagickPassFail BlackThresholdImage(Image *image,const char *thresholds)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o thresholds: Channel thresholds which are specified as a comma delimited
%      list containing the thresholds for red, green, blue, and opacity.  If
%      the list contains a percent symbol (%) then all values are treated as
%      a percentage of MaxRGB.
%
*/
MagickExport MagickPassFail BlackThresholdImage(Image *image,const char *thresholds)
{
  return QuantumOperatorImageMultivalue(image,ThresholdBlackQuantumOp,thresholds);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     B l u r I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlurImage() blurs an image.  We convolve the image with a Gaussian
%  operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and BlurImage() selects a suitable radius for you.
%
%  The format of the BlurImage method is:
%
%      Image *BlurImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define BlurImageColumnsText  "Blur image columns...  "
#define BlurImageRowsText  "Blur image rows...  "
static void BlurScanline(const double *kernel,const unsigned long width,
  const PixelPacket *source,PixelPacket *destination,
  const unsigned long columns)
{
  double
    scale;

  DoublePixelPacket
    aggregate,
    zero;

  register const double
    *p;

  register const PixelPacket
    *q;

  register long
    i,
    x;

  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  if (width > columns)
    {
      for (x=0; x < (long) columns; x++)
      {
        aggregate=zero;
        scale=0.0;
        p=kernel;
        q=source;
        for (i=0; i < (long) columns; i++)
        {
          if ((i >= (x-(long) width/2)) && (i <= (x+(long) width/2)))
            {
              aggregate.red+=(*p)*q->red;
              aggregate.green+=(*p)*q->green;
              aggregate.blue+=(*p)*q->blue;
              aggregate.opacity+=(*p)*q->opacity;
            }
          if (((i+(long)(width/2)-x) >= 0) && ((i+width/2-x) < width))
            scale+=kernel[i+width/2-x];
          p++;
          q++;
        }
        scale=1.0/scale;
        destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
        destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
        destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
        destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
      }
      return;
    }
  /*
    Blur scanline.
  */
  for (x=0; x < (long) (width/2); x++)
  {
    aggregate=zero;
    scale=0.0;
    p=kernel+width/2-x;
    q=source;
    for (i=width/2-x; i < (long) width; i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      aggregate.opacity+=(*p)*q->opacity;
      scale+=(*p);
      p++;
      q++;
    }
    scale=1.0/scale;
    destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
    destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
    destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
    destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
  }
  for ( ; x < (long) (columns-width/2); x++)
  {
    aggregate=zero;
    p=kernel;
    q=source+(x-width/2);
    for (i=0; i < (long) width; i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      aggregate.opacity+=(*p)*q->opacity;
      p++;
      q++;
    }
    destination[x].red=(Quantum) (aggregate.red+0.5);
    destination[x].green=(Quantum) (aggregate.green+0.5);
    destination[x].blue=(Quantum) (aggregate.blue+0.5);
    destination[x].opacity=(Quantum) (aggregate.opacity+0.5);
  }
  for ( ; x < (long) columns; x++)
  {
    aggregate=zero;
    scale=0;
    p=kernel;
    q=source+(x-width/2);
    for (i=0; i < (long) (columns-x+width/2); i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      aggregate.opacity+=(*p)*q->opacity;
      scale+=(*p);
      p++;
      q++;
    }
    scale=1.0/scale;
    destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
    destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
    destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
    destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
  }
}

static int GetBlurKernel(unsigned long width,const double sigma,double **kernel)
{
#define KernelRank 3

  double
    alpha,
    normalize;

  int
    bias;

  register long
    i;

  /*
    Generate a 1-D convolution matrix.  Calculate the kernel at higher
    resolution than needed and average the results as a form of numerical
    integration to get the best accuracy.
  */
  if (width == 0)
    width=3;
  *kernel=MagickAllocateMemory(double *,width*sizeof(double));
  if (*kernel == (double *) NULL)
    return(0);
  for (i=0; i < (long) width; i++)
    (*kernel)[i]=0.0;
  bias=KernelRank*width/2;
  for (i=(-bias); i <= bias; i++)
  {
    alpha=exp(-((double) i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
    (*kernel)[(i+bias)/KernelRank]+=alpha/(MagickSQ2PI*sigma);
  }
  normalize=0;
  for (i=0; i < (long) width; i++)
    normalize+=(*kernel)[i];
  for (i=0; i < (long) width; i++)
    (*kernel)[i]/=normalize;
  return(width);
}

static MagickPassFail BlurImageScanlines(Image *image,const double *kernel,
                                         const unsigned long width,
                                         const char *text,
                                         ExceptionInfo *exception)
{
  ThreadViewSet
    *view_set;

  MagickBool
    is_grayscale;

  volatile MagickPassFail
    status=MagickPass;

  is_grayscale=image->is_grayscale;
  view_set=AllocateThreadViewSet((Image *) image,exception);
  if (view_set == (ThreadViewSet *) NULL)
    status=MagickFail;

  if (status != MagickFail)
    {
      unsigned int
        i,
        views;

      views=GetThreadViewSetAllocatedViews(view_set);
      for (i=0; i < views; i++)
        {
          void
            *scanline;

          scanline=MagickAllocateMemory(PixelPacket *,image->columns*sizeof(PixelPacket));
          if (scanline == (PixelPacket *) NULL)
            {
              ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
                             MagickMsg(OptionError,UnableToBlurImage));
              status=MagickFail;
              break;
            }
          else
            {
              AssignThreadViewData(view_set,i,scanline);
            }
        }
    }

  if (status != MagickFail)
    {
      unsigned long
        row_count=0;

      long
        y;

#pragma omp parallel for schedule(static,64)
      for (y=0; y < (long) image->rows; y++)
        {
          ViewInfo
            view;

          register PixelPacket
            *q;

          PixelPacket
            *scanline;

          MagickBool
            thread_status;

          thread_status=status;
          if (thread_status == MagickFail)
            continue;
  
          scanline=AccessThreadViewData(view_set);
          view=AccessThreadView(view_set);
          q=GetCacheView(view,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            thread_status=MagickFail;

          if (thread_status != MagickFail)
            {
              (void) memcpy(scanline,q,image->columns*sizeof(PixelPacket));
              BlurScanline(kernel,width,scanline,q,image->columns);

              if (!SyncCacheView(view))
                {
                  thread_status=MagickFail;
                  CopyException(exception,&image->exception);
                }
            }
#pragma omp critical
          {
            row_count++;
            if (QuantumTick(row_count,image->rows))
              if (!MagickMonitor(text,row_count,image->rows,exception))
                thread_status=MagickFail;
          
            if (thread_status == MagickFail)
              status=MagickFail;
          }
        }
    }

  DestroyThreadViewSet(view_set);
  image->is_grayscale=is_grayscale;

  return status;
}

MagickExport Image *BlurImage(const Image *original_image,const double radius,
                              const double sigma,ExceptionInfo *exception)
{

  double
    *kernel;

  Image
    *blur_image;

  int
    width;

  MagickPassFail
    status=MagickPass;

  /*
    Get convolution matrix for the specified standard-deviation.
  */
  assert(original_image != (Image *) NULL);
  assert(original_image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  kernel=(double *) NULL;
  if (radius > 0)
    width=GetBlurKernel((int) (2*ceil(radius)+1),sigma,&kernel);
  else
    {
      double
        *last_kernel;

      last_kernel=(double *) NULL;
      width=GetBlurKernel(3,sigma,&kernel);
      while ((long) (MaxRGB*kernel[0]) > 0)
        {
          if (last_kernel != (double *)NULL)
            MagickFreeMemory(last_kernel);
          last_kernel=kernel;
          kernel=(double *) NULL;
          width=GetBlurKernel(width+2,sigma,&kernel);
        }
      if (last_kernel != (double *) NULL)
        {
          MagickFreeMemory(kernel);
          width-=2;
          kernel=last_kernel;
        }
    }
  if (width < 3)
    ThrowImageException3(OptionError,UnableToBlurImage,
                         KernelRadiusIsTooSmall);
  
  blur_image=RotateImage(original_image,90,exception);
  if (blur_image == (Image *) NULL)
    status=MagickFail;

  if (status != MagickFail)
    status&=BlurImageScanlines(blur_image,kernel,width,BlurImageColumnsText,exception);
  
  if (status != MagickFail)
  {
    Image
      *rotate_image;

    rotate_image=RotateImage(blur_image,-90,exception);
    if (rotate_image == (Image *) NULL)
      status=MagickFail;

    if (status != MagickFail)
      {
        DestroyImage(blur_image);
        blur_image=rotate_image;
      }
  }

  if (status != MagickFail)
    status&=BlurImageScanlines(blur_image,kernel,width,BlurImageRowsText,exception);

  MagickFreeMemory(kernel);
  blur_image->is_grayscale=original_image->is_grayscale;
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C h a n n e l T h r e s h o l d I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ChannelThresholdImage() changes the value of individual pixels based on
%  the level of each pixel channel.  The result sets the affected channels
%  to the minimum or maximum channel value. A negative threshold value
%  disables thresholding for that channel.  Append a percent symbol to
%  have threshold values automatically scaled from a percentage to MaxRGB.
%
%  Invoked by the '-threshold' option.
%
%  The format of the ChannelThresholdImage method is:
%
%      MagickPassFail ChannelThresholdImage(Image *image,const char *threshold)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o threshold: define the threshold values, <red>{<green>,<blue>,<opacity>}{%}.
%
%
*/
typedef struct _ChannelThresholdOptions_t
{
  PixelPacket
    thresholds;

  MagickBool
    red_enabled,
    green_enabled,
    blue_enabled,
    opacity_enabled;
} ChannelThresholdOptions_t;

static MagickPassFail
ChannelThresholdPixels(void *mutable_data,         /* User provided mutable data */
                       const void *immutable_data, /* User provided immutable data */
                       Image *image,               /* Modify image */
                       PixelPacket *pixels,        /* Pixel row */
                       IndexPacket *indexes,       /* Pixel row indexes */
                       const long npixels,         /* Number of pixels in row */
                       ExceptionInfo *exception)   /* Exception report */
{
  /*
    Threshold channels
  */
  const ChannelThresholdOptions_t
    *options = (const ChannelThresholdOptions_t *) immutable_data;

  register long
    i;

  const PixelPacket
    *thresholds=&options->thresholds;

  const MagickBool
    red_enabled=options->red_enabled,
    green_enabled=options->green_enabled,
    blue_enabled=options->blue_enabled,
    opacity_enabled=options->opacity_enabled;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      if (red_enabled)
        pixels[i].red=(pixels[i].red <= thresholds->red ? 0U : MaxRGB);
      if (green_enabled)
        pixels[i].green=(pixels[i].green <= thresholds->green ? 0U : MaxRGB);
      if (blue_enabled)
        pixels[i].blue=(pixels[i].blue <= thresholds->blue ? 0U : MaxRGB);
      if (opacity_enabled)
        pixels[i].opacity=(pixels[i].opacity <= thresholds->opacity ? 0U : MaxRGB);
    }
  return MagickPass;
}

#define ChannelThresholdImageText  "Channel threshold the image...  "
MagickExport MagickPassFail ChannelThresholdImage(Image *image,
  const char *threshold)
{
  DoublePixelPacket
    double_threshold;

  ChannelThresholdOptions_t
    options;

  int
    count;

  unsigned int
    is_grayscale=image->is_grayscale;

  MagickPassFail
    status = MagickPass;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  
  if (threshold == (const char *) NULL)
    return(MagickFail);

  options.thresholds.red     = 0U;
  options.thresholds.green   = 0U;
  options.thresholds.blue    = 0U;
  options.thresholds.opacity = 0U;

  options.red_enabled        = MagickFalse;
  options.green_enabled      = MagickFalse;
  options.blue_enabled       = MagickFalse;
  options.opacity_enabled    = MagickFalse;

  double_threshold.red       = -1.0;
  double_threshold.green     = -1.0;
  double_threshold.blue      = -1.0;
  double_threshold.opacity   = -1.0;
  count=sscanf(threshold,"%lf%*[/,%%]%lf%*[/,%%]%lf%*[/,%%]%lf",
               &double_threshold.red,
               &double_threshold.green,
               &double_threshold.blue,
               &double_threshold.opacity);
  
  if ((count > 3) && (double_threshold.opacity >= 0.0))
    options.opacity_enabled = MagickTrue;
  if ((count > 2) && (double_threshold.blue >= 0.0))
    options.blue_enabled = MagickTrue;
  if ((count > 1) && (double_threshold.green >= 0.0))
    options.green_enabled = MagickTrue;
  if ((count > 0) && (double_threshold.red >= 0.0))
    options.red_enabled = MagickTrue;
  
  if (strchr(threshold,'%') != (char *) NULL)
    {
      if (options.red_enabled)
        double_threshold.red     *= MaxRGB/100.0;
      if (options.green_enabled)
        double_threshold.green   *= MaxRGB/100.0;
      if (options.blue_enabled)
        double_threshold.blue    *= MaxRGB/100.0;
      if (options.opacity_enabled)
        double_threshold.opacity *= MaxRGB/100.0;
    }

  if (options.red_enabled)
    options.thresholds.red     = RoundDoubleToQuantum(double_threshold.red);
  if (options.green_enabled)
    options.thresholds.green   = RoundDoubleToQuantum(double_threshold.green);
  if (options.blue_enabled)
    options.thresholds.blue    = RoundDoubleToQuantum(double_threshold.blue);
  if (options.opacity_enabled)
    options.thresholds.opacity = RoundDoubleToQuantum(double_threshold.opacity);

  (void) SetImageType(image,TrueColorType);

  status=PixelIterateMonoModify(ChannelThresholdPixels,
                                NULL,
                                ChannelThresholdImageText,
                                NULL,&options,
                                0,0,image->columns,image->rows,
                                image,
                                &image->exception);

  if (is_grayscale && options.red_enabled && options.green_enabled && options.blue_enabled)
    {
      image->is_monochrome=True;
      image->is_grayscale=True;
    }
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D e s p e c k l e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Despeckle() reduces the speckle noise in an image while perserving the
%  edges of the original image.
%
%  The format of the DespeckleImage method is:
%
%      Image *DespeckleImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *DespeckleImage(const Image *image,ExceptionInfo *exception)
{
#define DespeckleImageText  "Despeckle image...  "

  Image
    *despeckle_image;

  int
    layer;

  long
    j,
    y;

  Quantum
    *buffer,
    *pixels;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    length;

  static const int
    X[4]= {0, 1, 1,-1},
    Y[4]= {1, 0, 1, 1};

  /*
    Allocate despeckled image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  despeckle_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (despeckle_image == (Image *) NULL)
    return((Image *) NULL);
  despeckle_image->storage_class=DirectClass;
  /*
    Allocate image buffers.
  */
  length=(image->columns+2)*(image->rows+2)*sizeof(Quantum);
  pixels=MagickAllocateMemory(Quantum *,length);
  buffer=MagickAllocateMemory(Quantum *,length);
  if ((buffer == (Quantum *) NULL) || (pixels == (Quantum *) NULL))
    {
      DestroyImage(despeckle_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToDespeckleImage)
    }
  /*
    Reduce speckle in the image.
  */
  for (layer=0; layer <= 3; layer++)
  {
    (void) memset(pixels,0,length);
    j=(long) image->columns+2;
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        break;
      j++;

      switch (layer)
        {
        case 0:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->red;
          break;
        case 1:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->green;
          break;
        case 2:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->blue;
          break;
        case 3:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->opacity;
          break;
        default: break;
        }

      j++;
    }
    (void) memset(buffer,0,length);
    for (i=0; i < 4; i++)
    {
      if (!MagickMonitor(DespeckleImageText,4*layer+i,15,exception))
        break;
      Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,1);
      Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,1);
      Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,-1);
      Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,-1);
    }
    j=(long) image->columns+2;
    for (y=0; y < (long) image->rows; y++)
    {
      q=GetImagePixels(despeckle_image,0,y,despeckle_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      j++;

      switch (layer)
        {
        case 0:
          for (x=(long) image->columns; x > 0; x--)
            q++->red=pixels[j++];
          break;
        case 1:
          for (x=(long) image->columns; x > 0; x--)
            q++->green=pixels[j++];
          break;
        case 2:
          for (x=(long) image->columns; x > 0; x--)
            q++->blue=pixels[j++];
          break;
        case 3:
          for (x=(long) image->columns; x > 0; x--)
            q++->opacity=pixels[j++];
          break;
        default: break;
        }

      if (!SyncImagePixels(despeckle_image))
        break;
      j++;
    }
  }
  /*
    Free memory.
  */
  MagickFreeMemory(buffer);
  MagickFreeMemory(pixels);
  despeckle_image->is_grayscale=image->is_grayscale;
  return(despeckle_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E d g e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EdgeImage() finds edges in an image.  Radius defines the radius of the
%  convolution filter.  Use a radius of 0 and Edge() selects a suitable
%  radius for you.
%
%  The format of the EdgeImage method is:
%
%      Image *EdgeImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EdgeImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *edge_image;

  int
    width;

  register long
    i;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToEdgeImage,
      ImageSmallerThanRadius);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToEdgeImage);
  for (i=0; i < (width*width); i++)
    kernel[i]=(-1.0);
  kernel[i/2]=width*width-1.0;
  edge_image=ConvolveImage(image,width,kernel,exception);
  MagickFreeMemory(kernel);
  edge_image->is_grayscale=image->is_grayscale;
  return(edge_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E m b o s s I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EmbossImage() returns a grayscale image with a three-dimensional effect.
%  We convolve the image with a Gaussian operator of the given radius and
%  standard deviation (sigma).  For reasonable results, radius should be
%  larger than sigma.  Use a radius of 0 and Emboss() selects a suitable
%  radius for you.
%
%  The format of the EmbossImage method is:
%
%      Image *EmbossImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EmbossImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel;

  Image
    *emboss_image;

  int
    j,
    width;

  register long
    i,
    u,
    v;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToEmbossImage);
  i=0;
  j=width/2;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=((u < 0) || (v < 0) ? -8.0 : 8.0)*
        alpha/(2.0*MagickPI*sigma*sigma);
      if (u == j)
        kernel[i]=v == j ? 1.0 : 0.0;
      i++;
    }
    j--;
  }
  emboss_image=ConvolveImage(image,width,kernel,exception);
  if (emboss_image != (Image *) NULL)
    (void) EqualizeImage(emboss_image);
  MagickFreeMemory(kernel);
  emboss_image->is_grayscale=image->is_grayscale;
  return(emboss_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E n h a n c e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EnhanceImage() applies a digital filter that improves the quality of a
%  noisy image.
%
%  The format of the EnhanceImage method is:
%
%      Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
{
#define Enhance(weight) \
  mean=((double) r->red+pixel.red)/2; \
  distance=r->red-(double) pixel.red; \
  distance_squared=(2.0*((double) MaxRGB+1.0)+mean)*distance*distance/MaxRGB; \
  mean=((double) r->green+pixel.green)/2; \
  distance=r->green-(double) pixel.green; \
  distance_squared+=4.0*distance*distance; \
  mean=((double) r->blue+pixel.blue)/2; \
  distance=r->blue-(double) pixel.blue; \
  distance_squared+= \
    (3.0*((double) MaxRGB+1.0)-1.0-mean)*distance*distance/MaxRGB; \
  mean=((double) r->opacity+pixel.opacity)/2; \
  distance=r->opacity-(double) pixel.opacity; \
  distance_squared+= \
    (3.0*((double) MaxRGB+1.0)-1.0-mean)*distance*distance/MaxRGB; \
  if (distance_squared < ((double) MaxRGB*MaxRGB/25.0)) \
    { \
      aggregate.red+=(weight)*r->red; \
      aggregate.green+=(weight)*r->green; \
      aggregate.blue+=(weight)*r->blue; \
      aggregate.opacity+=(weight)*r->opacity; \
      total_weight+=(weight); \
    } \
  r++;
#define EnhanceImageText  "  Enhance image...  "

  DoublePixelPacket
    aggregate,
    zero;

  double
    distance,
    distance_squared,
    mean,
    total_weight;

  Image
    *enhance_image;

  long
    y;

  PixelPacket
    pixel;

  register const PixelPacket
    *p,
    *r;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize enhanced image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 5) || (image->rows < 5))
    return((Image *) NULL);
  enhance_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (enhance_image == (Image *) NULL)
    return((Image *) NULL);
  enhance_image->storage_class=DirectClass;
  /*
    Enhance image.
  */
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    /*
      Read another scan line.
    */
    p=AcquireImagePixels(image,0,y-2,image->columns,5,exception);
    q=SetImagePixels(enhance_image,0,y,enhance_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    /*
      Transfer first 2 pixels of the scanline.
    */
    *q++=(*(p+2*image->columns));
    *q++=(*(p+2*image->columns+1));
    for (x=2; x < (long) (image->columns-2); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      aggregate=zero;
      total_weight=0.0;
      r=p+2*image->columns+2;
      pixel=(*r);
      r=p;
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
      r=p+image->columns;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      r=p+2*image->columns;
      Enhance(10); Enhance(40); Enhance(80); Enhance(40); Enhance(10);
      r=p+3*image->columns;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      r=p+4*image->columns;
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
      q->red=(Quantum) ((aggregate.red+(total_weight/2)-1)/total_weight);
      q->green=(Quantum) ((aggregate.green+(total_weight/2)-1)/total_weight);
      q->blue=(Quantum) ((aggregate.blue+(total_weight/2)-1)/total_weight);
      q->opacity=(Quantum)
        ((aggregate.opacity+(total_weight/2)-1)/total_weight);
      p++;
      q++;
    }
    p++;
    *q++=(*p++);
    *q++=(*p++);
    if (!SyncImagePixels(enhance_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(EnhanceImageText,y,image->rows-2,exception))
        break;
  }
  enhance_image->is_grayscale=image->is_grayscale;
  return(enhance_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a u s s i a n B l u r I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GaussianBlurImage() blurs an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and GaussianBlurImage() selects a suitable radius for you
%
%  The format of the BlurImage method is:
%
%      Image *GaussianBlurImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method GaussianBlurImage returns a pointer to the image
%      after it is blur.  A null image is returned if there is a memory
%      shortage.
%
%    o radius: the radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: the standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *GaussianBlurImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel;

  Image
    *blur_image;

  int
    width;

  register long
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth2D(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToBlurImage,
      ImageSmallerThanRadius);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitError,MemoryAllocationFailed,
      MagickMsg(OptionError,UnableToBlurImage));
  i=0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=alpha/(2.0*MagickPI*sigma*sigma);
      i++;
    }
  }
  blur_image=ConvolveImage(image,width,kernel,exception);
  MagickFreeMemory(kernel);
  blur_image->is_grayscale=image->is_grayscale;
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M e d i a n F i l t e r I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MedianFilterImage() applies a digital filter that improves the quality
%  of a noisy image.  Each pixel is replaced by the median in a set of
%  neighboring pixels as defined by radius.
%
%  The algorithm was contributed by Mike Edmonds and implements an insertion
%  sort for selecting median color-channel values.  For more on this algorithm
%  see "Skip Lists: A probabilistic Alternative to Balanced Trees" by William
%  Pugh in the June 1990 of Communications of the ACM.
%
%  The format of the MedianFilterImage method is:
%
%      Image *MedianFilterImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

typedef struct _MedianListNode
{
  unsigned long
    next[9],
    count,
    signature;
} MedianListNode;

typedef struct _MedianSkipList
{
  int
    level;

  MedianListNode
    *nodes;
} MedianSkipList;

typedef struct _MedianPixelList
{
  unsigned long
    center,
    seed,
    signature;

  MedianSkipList
    lists[4];
} MedianPixelList;

static void AddNodeMedianList(MedianPixelList *pixel_list,int channel,
  unsigned long color)
{
  register long
    level;

  register MedianSkipList
    *list;

  unsigned long
    search,
    update[9];

  /*
    Initialize the node.
  */
  list=pixel_list->lists+channel;
  list->nodes[color].signature=pixel_list->signature;
  list->nodes[color].count=1;
  /*
    Determine where it belongs in the list.
  */
  search=65536L;
  for (level=list->level; level >= 0; level--)
  {
    while (list->nodes[search].next[level] < color)
      search=list->nodes[search].next[level];
    update[level]=search;
  }
  /*
    Generate a pseudo-random level for this node.
  */
  for (level=0; ; level++)
  {
    pixel_list->seed=(pixel_list->seed*42893621L)+1L;
    if ((pixel_list->seed & 0x300) != 0x300)
      break;
  }
  if (level > 8)
    level=8;
  if (level > (list->level+2))
    level=list->level+2;
  /*
    If we're raising the list's level, link back to the root node.
  */
  while (level > list->level)
  {
    list->level++;
    update[list->level]=65536L;
  }
  /*
    Link the node into the skip-list.
  */
  do
  {
    list->nodes[color].next[level]=list->nodes[update[level]].next[level];
    list->nodes[update[level]].next[level]=color;
  }
  while (level-- > 0);
}

static PixelPacket GetMedianList(MedianPixelList *pixel_list)
{
  PixelPacket
    pixel;

  register long
    channel;

  register MedianSkipList
    *list;

  unsigned long
    center,
    channels[4],
    color,
    count;

  /*
    Find the median value for each of the color.
  */
  center=pixel_list->center;
  for (channel=0; channel < 4; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    count=0;
    do
    {
      color=list->nodes[color].next[0];
      count+=list->nodes[color].count;
    }
    while (count <= center);
    channels[channel]=color;
  }
  pixel.red=ScaleShortToQuantum(channels[0]);
  pixel.green=ScaleShortToQuantum(channels[1]);
  pixel.blue=ScaleShortToQuantum(channels[2]);
  pixel.opacity=ScaleShortToQuantum(channels[3]);
  return(pixel);
}

static int InitializeMedianList(MedianPixelList *pixel_list,long width)
{
  register int i;
  pixel_list->center=width*width/2;
  pixel_list->signature=MagickSignature;
  (void) memset((void *) pixel_list->lists,0,4*sizeof(MedianSkipList));
  for(i=0;i<4;i++)
  {
    pixel_list->lists[i].nodes = MagickAllocateMemory(MedianListNode *,65537 * sizeof(MedianListNode));
	if(!pixel_list->lists[i].nodes)
	{
	  assert(0);
	  return (False);
	}
  }
  return (True);
}

static void CleanMedianList(MedianPixelList *pixel_list)
{
  register int i;
  for(i=0;i<4;i++)
  {
	if(pixel_list->lists[i].nodes)
	  MagickFreeMemory(pixel_list->lists[i].nodes);
  }
}

static inline void InsertMedianList(MedianPixelList *pixel_list,
  const PixelPacket *pixel)
{
  unsigned long
    signature;

  unsigned int
    red_index,
    green_index,
    blue_index,
    opacity_index;

  red_index=ScaleQuantumToShort(pixel->red);
  signature=pixel_list->lists[0].nodes[red_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,0,red_index);
  else
    pixel_list->lists[0].nodes[red_index].count++;

  green_index=ScaleQuantumToShort(pixel->green);
  signature=pixel_list->lists[1].nodes[green_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,1,green_index);
  else
    pixel_list->lists[1].nodes[green_index].count++;

  blue_index=ScaleQuantumToShort(pixel->blue);
  signature=pixel_list->lists[2].nodes[blue_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,2,blue_index);
  else
    pixel_list->lists[2].nodes[blue_index].count++;

  opacity_index=ScaleQuantumToShort(pixel->opacity);
  signature=pixel_list->lists[3].nodes[opacity_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,3,opacity_index);
  else
    pixel_list->lists[3].nodes[opacity_index].count++;
}

static void ResetMedianList(MedianPixelList *pixel_list)
{
  int
    level;

  register long
    channel;

  register MedianListNode
    *root;

  register MedianSkipList
    *list;

  /*
    Reset the skip-list.
  */
  for (channel=0; channel < 4; channel++)
  {
    list=pixel_list->lists+channel;
    root=list->nodes+65536L;
    list->level=0;
    for (level=0; level < 9; level++)
      root->next[level]=65536L;
  }
  pixel_list->seed=pixel_list->signature++;
}

MagickExport Image *MedianFilterImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
#define MedianFilterImageText  "  Filter image with neighborhood ranking...  "

  Image
    *median_image;

  long
    width,
    x,
    y;

  MedianPixelList
    *skiplist;

  register const PixelPacket
    *p,
    *r;

  register long
    u,
    v;

  register PixelPacket
    *q;

  /*
    Initialize median image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToFilterImage,
      ImageSmallerThanRadius);
  median_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (median_image == (Image *) NULL)
    return((Image *) NULL);
  median_image->storage_class=DirectClass;
  /*
    Allocate skip-lists.
  */
  skiplist=MagickAllocateMemory(MedianPixelList *,sizeof(MedianPixelList));
  if (skiplist == (MedianPixelList *) NULL)
    {
      DestroyImage(median_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToMedianFilterImage)
    }
  /*
    Median filter each image row.
  */
  if(!InitializeMedianList(skiplist,width))
  {
    DestroyImage(median_image);
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
	  UnableToMedianFilterImage)
  }

  for (y=0; y < (long) median_image->rows; y++)
  {
    p=AcquireImagePixels(image,-width/2,y-width/2,image->columns+width,width,
      exception);
    q=SetImagePixels(median_image,0,y,median_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) median_image->columns; x++)
    {
      r=p;
      ResetMedianList(skiplist);
      for (v=0; v < width; v++)
      {
        for (u=0; u < width; u++)
          InsertMedianList(skiplist,r+u);
        r+=image->columns+width;
      }
      *q++=GetMedianList(skiplist);
      p++;
    }
    if (!SyncImagePixels(median_image))
      break;
    if (QuantumTick(y,median_image->rows))
      if (!MagickMonitor(MedianFilterImageText,y,median_image->rows,exception))
        break;
  }
  CleanMedianList(skiplist);
  MagickFreeMemory(skiplist);
  median_image->is_grayscale=image->is_grayscale;
  return(median_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o t i o n B l u r I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MotionBlurImage() simulates motion blur.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a
%  radius of 0 and MotionBlurImage() selects a suitable radius for you.
%  Angle gives the angle of the blurring motion.
%
%  Andrew Protano contributed this effect.
%
%  The format of the MotionBlurImage method is:
%
%    Image *MotionBlurImage(const Image *image,const double radius,
%      const double sigma,const double angle,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting
%     the center pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o angle: Apply the effect along this angle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static int GetMotionBlurKernel(int width,const double sigma,double **kernel)
{
#define KernelRank 3

  double
    alpha,
    normalize;

  int
    bias;

  register long
    i;

  /*
    Generate a 1-D convolution matrix.  Calculate the kernel at higher
    resolution than needed and average the results as a form of numerical
    integration to get the best accuracy.
  */
  if (width <= 0)
    width=3;
  *kernel=MagickAllocateMemory(double *,width*sizeof(double));
  if (*kernel == (double *) NULL)
    return(0);
  for (i=0; i < width; i++)
    (*kernel)[i]=0.0;
  bias=KernelRank*width;
  for (i=0; i < bias; i++)
  {
    alpha=exp(-((double) i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
    (*kernel)[i/KernelRank]+=alpha/(MagickSQ2PI*sigma);
  }
  normalize=0;
  for (i=0; i < width; i++)
    normalize+=(*kernel)[i];
  for (i=0; i < width; i++)
    (*kernel)[i]/=normalize;
  return(width);
}

#define MotionBlurImageText  "Motion blur image...  "
MagickExport Image *MotionBlurImage(const Image *image,const double radius,
  const double sigma,const double angle,ExceptionInfo *exception)
{
  double
    *kernel;

  DoublePixelPacket
    aggregate,
    zero;

  Image
    *blur_image;

  int
    width;

  long
    y;

  PixelPacket
    pixel;

  PointInfo
    *offsets;

  register long
    i,
    x,
    u,
    v;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  kernel=(double *) NULL;
  if (radius > 0)
    width=GetMotionBlurKernel((int) (2.0*ceil(radius)+1.0),sigma,&kernel);
  else
    {
      double
        *last_kernel;

      last_kernel=(double *) NULL;
      width=GetMotionBlurKernel(3,sigma,&kernel);
      while ((MaxRGB*kernel[width-1]) > 0.0)
      {
        if (last_kernel != (double *)NULL)
          MagickFreeMemory(last_kernel);
        last_kernel=kernel;
        kernel=(double *) NULL;
        width=GetMotionBlurKernel(width+2,sigma,&kernel);
      }
      if (last_kernel != (double *) NULL)
        {
          MagickFreeMemory(kernel);
          width-=2;
          kernel=last_kernel;
        }
    }
  if (width < 3)
    ThrowImageException3(OptionError,UnableToBlurImage,
      KernelRadiusIsTooSmall);
  offsets=MagickAllocateMemory(PointInfo *,width*sizeof(PointInfo));
  if (offsets == (PointInfo *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToMotionBlurImage);
  /*
    Allocate blur image.
  */
  blur_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (blur_image == (Image *) NULL)
    {
      MagickFreeMemory(kernel);
      MagickFreeMemory(offsets);
      return((Image *) NULL);
    }
  blur_image->storage_class=DirectClass;
  x=(long) (width*sin(DegreesToRadians(angle)));
  y=(long) (width*cos(DegreesToRadians(angle)));
  for (i=0; i < width; i++)
  {
    offsets[i].x=i*x/sqrt(x*x+y*y);
    offsets[i].y=i*y/sqrt(x*x+y*y);
  }
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(blur_image,0,y,blur_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      aggregate=zero;
      for (i=0; i < width; i++)
      {
        u=x+(long) offsets[i].x;
        v=y+(long) offsets[i].y;
        if ((u < 0) || (u >= (long) image->columns) ||
            (v < 0) || (v >= (long) image->rows))
          continue;
        pixel=AcquireOnePixel(image,u,v,exception);
        aggregate.red+=kernel[i]*pixel.red;
        aggregate.green+=kernel[i]*pixel.green;
        aggregate.blue+=kernel[i]*pixel.blue;
        aggregate.opacity+=kernel[i]*pixel.opacity;
      }
      q->red=(Quantum) aggregate.red;
      q->green=(Quantum) aggregate.green;
      q->blue=(Quantum) aggregate.blue;
      q->opacity=(Quantum) aggregate.opacity;
      q++;
    }
    if (!SyncImagePixels(blur_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(MotionBlurImageText,y,image->rows,exception))
        break;
  }
  MagickFreeMemory(kernel);
  MagickFreeMemory(offsets);
  blur_image->is_grayscale=image->is_grayscale;
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R a n d o m C h a n n e l T h r e s h o l d I m a g e                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RandomChannelThresholdImage() changes the value of individual pixels based
%  on the intensity of each pixel compared to a random threshold.  The result
%  is a low-contrast, two color image.
%
%  The format of the RandomChannelThresholdImage method is:
%
%      unsigned int RandomChannelThresholdImage(Image *image,
%         const char *channel, const char *thresholds,
%         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: The channel or channels to be thresholded.
%
%    o thresholds: a geometry string containing LOWxHIGH thresholds.
%      If the string contains 2x2, 3x3, or 4x4, then an ordered
%      dither of order 2, 3, or 4 will be performed instead.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport unsigned int RandomChannelThresholdImage(Image *image,const char
    *channel,const char *thresholds,ExceptionInfo *exception)
{
#define RandomChannelThresholdImageText  "Random-channel threshold image...  "

  const double
    o2[4]={.2,.6,.8,.4},
    o3[9]={.1,.6,.3,.7,.5,.8,.4,.9,.2},
    o4[16]={.1,.7,1.1,.3,1.0,.5,1.5,.8,1.4,1.6,.6,1.2,.4,.9,1.3,.2};

  double
    lower_threshold,
    threshold=128,
    upper_threshold;

  register IndexPacket
    index;

  long
    count,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  unsigned long
    logging,
    order;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  if (image->is_monochrome && !image->matte)
    return(True);
  if (thresholds == (const char *) NULL)
    return(True);

  if (LocaleCompare(thresholds,"2x2") == 0)
    order=2;
  else if (LocaleCompare(thresholds,"3x3") == 0)
    order=3;
  else if (LocaleCompare(thresholds,"4x4") == 0)
    order=4;
  else
    {
      order=1;
      lower_threshold=0;
      upper_threshold=0;
      count=sscanf(thresholds,"%lf[/x%%]%lf",
        &lower_threshold,&upper_threshold);

      if (strchr(thresholds,'%') != (char *) NULL)
        {
          upper_threshold*=(.01*MaxRGB);
          lower_threshold*=(.01*MaxRGB);
        }
      if (count == 1)
        upper_threshold=MaxRGB-lower_threshold;
    }

  logging=LogMagickEvent(TransformEvent,GetMagickModule(),
      "  RandomChannelThresholdImage: channel type=%s",channel);
  if (logging)
    (void)LogMagickEvent(TransformEvent,GetMagickModule(),
        "    Thresholds: %s (%fx%f)",
        thresholds,lower_threshold,upper_threshold);

  if (LocaleCompare(channel,"all") == 0 ||
      LocaleCompare(channel,"intensity") == 0)
    if (!AllocateImageColormap(image,2))
      ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToThresholdImage);

  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    if (LocaleCompare(channel,"all") == 0 ||
        LocaleCompare(channel,"intensity") == 0)
      {
        if (!image->is_monochrome)
          {
            if (image->is_grayscale)
              {
                for (x=(long) image->columns; x > 0; x--)
                  {
                    if (order == 1)
                      {
                        if ((double) q->red < lower_threshold)
                          threshold=lower_threshold;
                        else if ((double) q->red > upper_threshold)
                          threshold=upper_threshold;
                        else
                          threshold=(double) (MaxRGB*rand()/(double) RAND_MAX);
                      }
                    else if (order == 2)
                      threshold=(double) MaxRGB*o2[(x%2)+2*(y%2)];
                    else if (order == 3)
                      threshold=(double) MaxRGB*o3[(x%3)+3*(y%3)];
                    else if (order == 4)
                      threshold=(double) MaxRGB*o4[(x%4)+4*(y%4)]/1.7;
                    index=(double) q->red <= threshold ? 0 : 1;
                    *indexes++=index;
                    q->red=q->green=q->blue=image->colormap[index].red;
                    q++;
                  }
              }
            else
              {
                for (x=(long) image->columns; x > 0; x--)
                  {
                    double
                      intensity;
      
                    intensity=(double) PixelIntensityToQuantum(q);
                    if (order == 1)
                      {
                        if (intensity < lower_threshold)
                          threshold=lower_threshold;
                        else if (intensity > upper_threshold)
                          threshold=upper_threshold;
                        else
                          threshold=(double) (MaxRGB*(rand()
                              /(double) RAND_MAX));
                      }
                    else if (order == 2)
                      threshold=(double) MaxRGB*o2[(x%2)+2*(y%2)];
                    else if (order == 3)
                      threshold=(double) MaxRGB*o3[(x%3)+3*(y%3)];
                    else if (order == 4)
                      threshold=(double) MaxRGB*o4[(x%4)+4*(y%4)]/1.7;

                    index=intensity <= threshold ? 0 : 1;
                    *indexes++=index;
                    q->red=q->green=q->blue=image->colormap[index].red;
                    q++;
                  }
              }
          }
      }
    if (LocaleCompare(channel,"opacity") == 0 ||
        LocaleCompare(channel,"all") == 0 ||
        LocaleCompare(channel,"matte") == 0)
      {
        if (image->matte)
          for (x=(long) image->columns; x > 0; x--)
            {
              if (order == 1)
                {
                  if ((double) q->opacity < lower_threshold)
                    threshold=lower_threshold;
                  else if ((double) q->opacity > upper_threshold)
                    threshold=upper_threshold;
                  else
                    threshold=(double) (MaxRGB*(rand()/(double) RAND_MAX));
                }
              else if (order == 2)
                threshold=(double) MaxRGB*o2[(x%2)+2*(y%2)];
              else if (order == 3)
                threshold=(double) MaxRGB*o3[(x%3)+3*(y%3)];
              else if (order == 4)
                threshold=(double) MaxRGB*o4[(x%4)+4*(y%4)]/1.7;
              q->opacity=(Quantum) ((double) q->opacity <= threshold ?
                 0 : MaxRGB);
              q++;
            }
      }
    else
      {
        /* To Do: red, green, blue, cyan, magenta, yellow, black */
        if (LocaleCompare(channel,"intensity") != 0)
          ThrowBinaryException3(OptionError, UnableToThresholdImage,
              UnrecognizedChannelType);
      }

    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(RandomChannelThresholdImageText,y,image->rows,
          exception))
        break;
  }
  if (LocaleCompare(channel,"all") == 0 ||
      LocaleCompare(channel,"intensity") == 0)
    {
      image->is_monochrome=True;
      image->is_grayscale=True;
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R e d u c e N o i s e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReduceNoiseImage() smooths the contours of an image while still preserving
%  edge information.  The algorithm works by replacing each pixel with its
%  neighbor closest in value.  A neighbor is defined by radius.  Use a radius
%  of 0 and ReduceNoise() selects a suitable radius for you.
%
%  The format of the ReduceNoiseImage method is:
%
%      Image *ReduceNoiseImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static PixelPacket GetNonpeakMedianList(MedianPixelList *pixel_list)
{
  PixelPacket
    pixel;

  register MedianSkipList
    *list;

  register long
    channel;

  unsigned long
    channels[4],
    center,
    color,
    count,
    previous,
    next;

  /*
    Finds the median value for each of the color.
  */
  center=pixel_list->center;
  for (channel=0; channel < 4; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    next=list->nodes[color].next[0];
    count=0;
    do
    {
      previous=color;
      color=next;
      next=list->nodes[color].next[0];
      count+=list->nodes[color].count;
    }
    while (count <= center);
    if ((previous == 65536L) && (next != 65536L))
      color=next;
    else
      if ((previous != 65536L) && (next == 65536L))
        color=previous;
    channels[channel]=color;
  }
  pixel.red=ScaleShortToQuantum(channels[0]);
  pixel.green=ScaleShortToQuantum(channels[1]);
  pixel.blue=ScaleShortToQuantum(channels[2]);
  pixel.opacity=ScaleShortToQuantum(channels[3]);
  return(pixel);
}

MagickExport Image *ReduceNoiseImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
#define ReduceNoiseImageText  "Reduce the image noise...  "

  Image
    *noise_image;

  long
    width,
    x,
    y;

  MedianPixelList
    *skiplist;

  register const PixelPacket
    *p,
    *r;

  register long
    u,
    v;

  register PixelPacket
    *q;

  /*
    Initialize noised image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToFilterImage,
      ImageSmallerThanRadius);
  noise_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  noise_image->storage_class=DirectClass;
  /*
    Allocate skip-lists.
  */
  skiplist=MagickAllocateMemory(MedianPixelList *,sizeof(MedianPixelList));
  if (skiplist == (MedianPixelList *) NULL)
    {
      DestroyImage(noise_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToNoiseFilterImage)
    }
  /*
    Median filter each image row.
  */
  if(!InitializeMedianList(skiplist,width))
  {
    DestroyImage(noise_image);
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
	  UnableToMedianFilterImage)
  }

  for (y=0; y < (long) noise_image->rows; y++)
  {
    p=AcquireImagePixels(image,-width/2,y-width/2,image->columns+width,width,
      exception);
    q=SetImagePixels(noise_image,0,y,noise_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) noise_image->columns; x++)
    {
      r=p;
      ResetMedianList(skiplist);
      for (v=width; v > 0; v--)
      {
        register const PixelPacket
          *ru=r;
        for (u=width; u > 0; u--)
          InsertMedianList(skiplist,ru++);
        r+=image->columns+width;
      }
      *q++=GetNonpeakMedianList(skiplist);
      p++;
    }
    if (!SyncImagePixels(noise_image))
      break;
    if (QuantumTick(y,noise_image->rows))
      if (!MagickMonitor(ReduceNoiseImageText,y,noise_image->rows,exception))
        break;
  }
  CleanMedianList(skiplist);
  MagickFreeMemory(skiplist);
  noise_image->is_grayscale=image->is_grayscale;
  return(noise_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ShadeImage() shines a distant light on an image to create a
%  three-dimensional effect. You control the positioning of the light with
%  azimuth and elevation; azimuth is measured in degrees off the x axis
%  and elevation is measured in pixels above the Z axis.
%
%  The format of the ShadeImage method is:
%
%      Image *ShadeImage(const Image *image,const unsigned int gray,
%        double azimuth,double elevation,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o gray: A value other than zero shades the intensity of each pixel.
%
%    o azimuth, elevation:  Define the light source direction.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

MagickExport Image *ShadeImage(const Image *image,const unsigned int gray,
  double azimuth,double elevation,ExceptionInfo *exception)
{
#define ShadeImageText  "Shade image...  "

  double
    distance,
    normal_distance,
    shade;

  Image
    *shade_image;

  long
    y;

  PrimaryInfo
    light,
    normal;

  register const PixelPacket
    *p,
    *s0,
    *s1,
    *s2;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize shaded image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  shade_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (shade_image == (Image *) NULL)
    return((Image *) NULL);
  shade_image->storage_class=DirectClass;
  /*
    Compute the light vector.
  */
  azimuth=DegreesToRadians(azimuth);
  elevation=DegreesToRadians(elevation);
  light.x=(double) MaxRGB*cos(azimuth)*cos(elevation);
  light.y=(double) MaxRGB*sin(azimuth)*cos(elevation);
  light.z=(double) MaxRGB*sin(elevation);
  normal.z=2.0*MaxRGB;  /* constant Z of surface normal */
  /*
    Shade image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,-1,y-1,image->columns+2,3,exception);
    q=SetImagePixels(shade_image,0,y,shade_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    /*
      Shade this row of pixels.
    */
    s0=p+1;
    s1=s0+image->columns+2;
    s2=s1+image->columns+2;
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Determine the surface normal and compute shading.
      */
      normal.x=PixelIntensityToDouble(s0-1)+ PixelIntensityToDouble(s1-1)+
        PixelIntensityToDouble(s2-1)-PixelIntensityToDouble(s0+1)-
        PixelIntensityToDouble(s1+1)-PixelIntensityToDouble(s2+1);
      normal.y=PixelIntensityToDouble(s2-1)+PixelIntensityToDouble(s2)+
        PixelIntensityToDouble(s2+1)-PixelIntensityToDouble(s0-1)-
        PixelIntensityToDouble(s0)-PixelIntensityToDouble(s0+1);
      if ((normal.x == 0.0) && (normal.y == 0.0))
        shade=light.z;
      else
        {
          shade=0.0;
          distance=normal.x*light.x+normal.y*light.y+normal.z*light.z;
          if (distance > MagickEpsilon)
            {
              normal_distance=
                normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
              if (normal_distance > (MagickEpsilon*MagickEpsilon))
                shade=distance/sqrt(normal_distance);
            }
        }
      if (gray)
        {
          q->red=(Quantum) shade;
          q->green=(Quantum) shade;
          q->blue=(Quantum) shade;
        }
      else
        {
          q->red=(Quantum) ((shade*s1->red)/MaxRGB+0.5);
          q->green=(Quantum) ((shade*s1->green)/MaxRGB+0.5);
          q->blue=(Quantum) ((shade*s1->blue)/MaxRGB+0.5);
        }
      q->opacity=s1->opacity;
      s0++;
      s1++;
      s2++;
      q++;
    }
    if (!SyncImagePixels(shade_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ShadeImageText,y,image->rows,exception))
        break;
  }
  shade_image->is_grayscale=image->is_grayscale;
  if (gray)
    shade_image->is_grayscale=True;
  return(shade_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a r p e n I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SharpenImage() sharpens an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a
%  radius of 0 and SharpenImage() selects a suitable radius for you.
%
%  The format of the SharpenImage method is:
%
%    Image *SharpenImage(const Image *image,const double radius,
%      const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Laplacian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SharpenImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel,
    normalize;

  Image
    *sharp_image;

  long
    width;

  register long
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToSharpenImage,
      ImageSmallerThanRadius);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToSharpenImage);
  i=0;
  normalize=0.0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=alpha/(2.0*MagickPI*sigma*sigma);
      normalize+=kernel[i];
      i++;
    }
  }
  kernel[i/2]=(-2.0)*normalize;
  sharp_image=ConvolveImage(image,width,kernel,exception);
  MagickFreeMemory(kernel);
  sharp_image->is_grayscale=image->is_grayscale;
  return(sharp_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S p r e a d I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SpreadImage() is a special effects method that randomly displaces each
%  pixel in a block defined by the radius parameter.
%
%  The format of the SpreadImage method is:
%
%      Image *SpreadImage(const Image *image,const unsigned int radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius:  Choose a random pixel in a neighborhood of this extent.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SpreadImage(const Image *image,const unsigned int radius,
  ExceptionInfo *exception)
{
#define SpreadImageText  "Spread image...  "
#define OFFSETS_ENTRIES 5000

  Image
    *spread_image;

  int
    offsets_index;

  int
    x_distance,
    y,
    y_distance;

  register int
    x;

  long
    *offsets;

  register PixelPacket
    *q;

  const PixelPacket
    *neighbors;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 3) || (image->rows < 3))
    return((Image *) NULL);
  /*
    Initialize spread image attributes.
  */
  spread_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (spread_image == (Image *) NULL)
    return((Image *) NULL);
  spread_image->storage_class=DirectClass;
  offsets_index=0;

  /*
    Initialize random offsets cache
  */
  offsets=MagickAllocateMemory(long *,OFFSETS_ENTRIES*sizeof(long));
  if (offsets == (long *) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,NULL);
      return (Image *) NULL;
    }
  for (x=0; x < OFFSETS_ENTRIES; x++)
    offsets[x]=((((2*(double) radius+1)*rand())/RAND_MAX)-((long)radius));

  /*
    Convolve each row.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    long
      y_min,
      y_max;

    q=SetImagePixels(spread_image,0,y,spread_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;

    if (radius > (unsigned int) y)
      y_min=0;
    else
      y_min=y-radius;
    
    if ((y+radius) >= image->rows)
      y_max=image->rows-1;
    else
      y_max=y+radius;

    neighbors=AcquireImagePixels(image,0,y_min,image->columns,y_max-y_min,exception);
    if (neighbors == (PixelPacket *) NULL)
      break;

    for (x=0; x < (long) image->columns; x++)
    {
      do
      {
        x_distance=offsets[offsets_index++];
        if (offsets_index==OFFSETS_ENTRIES)
          offsets_index=0;
      } while (((x+x_distance) < 0) ||
               ((x+x_distance) >= (long) image->columns));
      do
      {
        y_distance=offsets[offsets_index++];
        if (offsets_index==OFFSETS_ENTRIES)
          offsets_index=0;
      } while (((y+y_distance) < 0) ||
               ((y+y_distance) >= (long) image->rows));
      *q=*(neighbors+(x+x_distance)+((y+y_distance-y_min)*image->columns));
      q++;
    }
    if (!SyncImagePixels(spread_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(SpreadImageText,y,image->rows,exception))
        break;
  }
  MagickFreeMemory(offsets);
  spread_image->is_grayscale=image->is_grayscale;
  spread_image->is_monochrome=image->is_monochrome;
  return(spread_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T h r e s h o l d I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ThresholdImage() changes the value of individual pixels based on
%  the intensity of each pixel compared to a specified threshold.  Values
%  greater than the threshold are set to the maximum quantum value, and
%  values equal to or below the threshold are set to the minimum quantum
%  value.  The result is a high-contrast, two color image.
%
%  The format of the ThresholdImage method is:
%
%      MagickPassFail ThresholdImage(Image *image,const double threshold)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o threshold: Define the threshold value
%
%
*/
MagickExport MagickPassFail ThresholdImage(Image *image,const double threshold)
{
#define ThresholdImageText  "Threshold the image...  "

  register IndexPacket
    index;

  unsigned long
    y;

  register IndexPacket
    *indexes;

  Quantum
    intensity,
    quantum_threshold;

  register unsigned long
    x;

  register PixelPacket
    *q;

  MagickBool
    grayscale,
    initialize_indexes,
    modified;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  initialize_indexes=(image->storage_class == PseudoClass ? MagickFalse : MagickTrue);
  grayscale=image->is_grayscale;
  quantum_threshold=RoundDoubleToQuantum(threshold);

  /*
    Check if image is already in desired state.
  */
  if ((quantum_threshold != MaxRGB) && (image->storage_class == PseudoClass)
      && (image->colors == 2))
    {
      if (IsBlackPixel(image->colormap[0]) && (IsWhitePixel(image->colormap[1])))
        {
          image->is_monochrome=MagickTrue;
          image->is_grayscale=MagickTrue;
          return MagickPass;
        }
    }

  if (!AllocateImageColormap(image,2))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                          UnableToThresholdImage);

  for (y=0; y < image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      modified=MagickFalse;

      for (x=0; x < image->columns; x++)
        {
          if (grayscale)
            intensity=q[x].red;
          else
            intensity=PixelIntensityToQuantum(&q[x]);
          index=(intensity <= quantum_threshold ? 0U : 1U);
          if ((initialize_indexes) || (index != indexes[x]))
            {
              modified=MagickTrue;
              indexes[x]=index;
            }
          if (NotColorMatch(&image->colormap[index],&q[x]))
            {
              modified=MagickTrue;
              q[x].red=q[x].green=q[x].blue=image->colormap[index].red;
            }
        }
      if (modified)
        if (!SyncImagePixels(image))
          break;
      if (QuantumTick(y,image->rows))
        if (!MagickMonitor(ThresholdImageText,y,image->rows,&image->exception))
          break;
    }
  image->is_monochrome=MagickTrue;
  image->is_grayscale=MagickTrue;
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     U n s h a r p M a s k I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnsharpMaskImage() sharpens an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a radius
%  of 0 and UnsharpMaskImage() selects a suitable radius for you.
%
%  The format of the UnsharpMaskImage method is:
%
%    Image *UnsharpMaskImage(const Image *image,const double radius,
%      const double sigma,const double amount,const double threshold,
%      ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o amount: The percentage of the difference between the original and the
%      blur image that is added back into the original.
%
%    o threshold: The threshold in pixels needed to apply the diffence amount.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

typedef struct _UnsharpMaskOptions_t
{
  double amount;    /* Difference multiplier */
  double threshold; /* Scaled to MaxRGB/2 */
} UnsharpMaskOptions_t;

static inline Quantum UnsharpQuantum(const Quantum original, const Quantum sharpened,
                                     const UnsharpMaskOptions_t* options)
{
  double
    value;

  Quantum
    quantum;

  quantum=original;
  value=original-(double) sharpened;
  if (AbsoluteValue(value) >= options->threshold)
    {
      value=original+(value*options->amount);
      quantum=RoundDoubleToQuantum(value);
    }

  return quantum;
}

static MagickPassFail
UnsharpMaskPixels(void *mutable_data,                /* User provided mutable data */
                  const void *immutable_data,        /* User provided immutable data */
                  const Image *source_image,         /* Source image */
                  const PixelPacket *source_pixels,  /* Pixel row in source image */
                  const IndexPacket *source_indexes, /* Pixel row indexes in source image */
                  Image *update_image,               /* Update image */
                  PixelPacket *update_pixels,        /* Pixel row in update image */
                  IndexPacket *update_indexes,       /* Pixel row indexes in update image */
                  const long npixels,                /* Number of pixels in row */
                  ExceptionInfo *exception           /* Exception report */
                  )
{
  const UnsharpMaskOptions_t
    *options = (const UnsharpMaskOptions_t *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(source_image);
  ARG_NOT_USED(source_indexes);
  ARG_NOT_USED(update_image);
  ARG_NOT_USED(update_indexes);
  ARG_NOT_USED(exception);
  for (i=0; i < npixels; i++)
    {
      update_pixels[i].red=UnsharpQuantum(source_pixels[i].red,update_pixels[i].red,options);
      update_pixels[i].green=UnsharpQuantum(source_pixels[i].green,update_pixels[i].green,options);
      update_pixels[i].blue=UnsharpQuantum(source_pixels[i].blue,update_pixels[i].blue,options);
      update_pixels[i].opacity=UnsharpQuantum(source_pixels[i].opacity,update_pixels[i].opacity,options);
    }
  return MagickPass;
}

#define SharpenImageText  "Sharpen image...  "
MagickExport Image *UnsharpMaskImage(const Image *image,const double radius,
  const double sigma,const double amount,const double threshold,
  ExceptionInfo *exception)
{
  UnsharpMaskOptions_t
    options;

  Image
    *sharp_image;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  sharp_image=GaussianBlurImage(image,radius,sigma,exception);
  if (sharp_image == (Image *) NULL)
    return((Image *) NULL);
  options.amount=amount;
  options.threshold=(MaxRGBFloat*threshold)/2.0;
  (void) PixelIterateDualModify(UnsharpMaskPixels,NULL,
                                SharpenImageText,NULL,&options,
                                image->columns,image->rows,image,0,0,sharp_image,
                                0,0,exception);                                
  sharp_image->is_grayscale=image->is_grayscale;
  return(sharp_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     W h i t e T h r e s h o l d I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WhiteThresholdImage() adjusts the levels of image channels such that
%  values above a specified threshold are set to the maximum value (white)
%  while the remaining pixels are unchanged.
%
%  The format of the WhiteThresholdImage method is:
%
%      MagickPassFail WhiteThresholdImage(Image *image,const char *thresholds)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o thresholds: Channel thresholds which are specified as a comma delimited
%      list containing the thresholds for red, green, blue, and opacity.  If
%      the list contains a percent symbol (%) then all values are treated as
%      a percentage of MaxRGB.
%
*/
MagickExport MagickPassFail WhiteThresholdImage(Image *image,const char *thresholds)
{
  return QuantumOperatorImageMultivalue(image,ThresholdWhiteQuantumOp,thresholds);
}
