/*
% Copyright (C) 2004 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%            CCCC  H   H   AAA   N   N  N   N  EEEEE  L                       %
%           C      H   H  A   A  NN  N  NN  N  E      L                       %
%           C      HHHHH  AAAAA  N N N  N N N  EEE    L                       %
%           C      H   H  A   A  N  NN  N  NN  E      L                       %
%            CCCC  H   H  A   A  N   N  N   N  EEEEE  LLLLL                   %
%                                                                             %
%                                                                             %
%                       Image Channel Operations                              %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                           Bob Friesenhahn                                   %
%                             July  2004                                      %
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
#include "magick/cache.h"
#include "magick/channel.h"
#include "magick/image.h"
#include "magick/monitor.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     C h a n n e l I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Transform an image so that the resulting image is a grayscale image
%  based on a specified image channel. The resulting image is returned in
%  the RGB colorspace. This function does not force or assume an input
%  image colorspace so it may be used to extract channels from images in
%  colorspaces other than RGB or CMYK. For example, if the image is currently
%  transformed to the HWB colorspace, the 'B' channel may be extracted by
%  specifying RedChannel as the ChannelType argument.
%
%  The format of the ChannelImage method is:
%
%      unsigned int ChannelImage(Image *image,const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Identify which channel to extract: Red, Cyan, Green, Magenta,
%    Blue, Yellow, or Opacity.
%
%
*/
MagickExport MagickPassFail ChannelImage(Image *image,const ChannelType channel)
{
#define ChannelImageText  "  Extract a channel from the image...  "

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  /*
    Channel DirectClass packets.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == PseudoClass)
    {
      status &= SyncImage(image);
      image->storage_class=DirectClass;
    }
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    switch (channel)
      {
      case RedChannel:
      case CyanChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->green=q->red;
              q->blue=q->red;
              q++;
            }
          break;
        }
      case GreenChannel:
      case MagentaChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=q->green;
              q->blue=q->green;
              q++;
            }
          break;
        }
      case BlueChannel:
      case YellowChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=q->blue;
              q->green=q->blue;
              q++;
            }
          break;
        }
      case OpacityChannel:
        {
          if (image->colorspace == CMYKColorspace)
            {
              indexes=GetIndexes(image);
              if (!indexes)
                {
                  status=MagickFail;
                  break;
                }
              for (x=(long) image->columns; x > 0; x--)
                {
                  q->red=*indexes;
                  q->green=*indexes;
                  q->blue=*indexes;
                  q->opacity=OpaqueOpacity;
                  q++;
                  indexes++;
                }
            }
          else
            {
              for (x=(long) image->columns; x > 0; x--)
                {
                  q->red=q->opacity;
                  q->green=q->opacity;
                  q->blue=q->opacity;
                  q->opacity=OpaqueOpacity;
                  q++;
                }
            }
          image->matte=False;
          break;
        }
      case MatteChannel:
      case BlackChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=q->opacity;
              q->green=q->opacity;
              q->blue=q->opacity;
              q->opacity=OpaqueOpacity;
              q++;
            }
          image->matte=False;
          break;
        }
      default:
        {
        }
      }
    if (!SyncImagePixels(image))
      {
        status=MagickFail;
        break;
      }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ChannelImageText,y,image->rows,&image->exception))
        break;
  }
  image->colorspace=RGBColorspace;
  image->is_grayscale=True;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     E x p o r t I m a g e C h a n n e l                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExportImageChannel() exports a specified image channel as a new image.
%
%  The format of the ExportImageChannel method is:
%
%      Image *ExportImageChannel(const Image *image,
%                                const ChannelType channel,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The source image.
%
%    o channel: The image channel to export
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define EXPORT_IMAGE_CHANNEL(src_image,dst_image,source) \
do { \
  if (src_image->storage_class == PseudoClass) \
    { \
      register const IndexPacket \
        *src_indexes; \
      \
      src_indexes=GetIndexes(src_image); \
      for (x=(long) dst_image->columns; x > 0; x--) \
        { \
          q->red=q->green=q->blue=src_image->colormap[*src_indexes].source; \
          q->opacity=OpaqueOpacity; \
          src_indexes++; \
          q++; \
        } \
    } \
  else \
    { \
      for (x=(long) dst_image->columns; x > 0; x--) \
        { \
          q->red=q->green=q->blue=p++->source; \
          q->opacity=OpaqueOpacity; \
          q++; \
        } \
    } \
} while (0);

MagickExport Image *ExportImageChannel(const Image *src_image,
                                       const ChannelType channel,
                                       ExceptionInfo *exception)
{
  Image
    *dst_image;

  long
    y;

/*   register IndexPacket */
/*     *dst_indexes; */

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  assert(src_image != (Image *) NULL);
  assert(src_image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  dst_image=CloneImage(src_image,src_image->columns,src_image->rows,True,exception);
  if (dst_image == (Image *) NULL)
    {
      return ((Image *) NULL);
    }
  dst_image->storage_class=DirectClass;

  /* AllocateImageColormap(dst_image,MaxColormapSize); */

  for (y=0; y < (long) dst_image->rows; y++)
    {
      q=SetImagePixels(dst_image,0,y,dst_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      /* dst_indexes=GetIndexes(dst_image); */

      p=AcquireImagePixels(src_image,0,y,dst_image->columns,1,exception);
      if (q == (const PixelPacket *) NULL)
        break;

      switch (channel)
        {
        case RedChannel:
        case CyanChannel:
          {
            EXPORT_IMAGE_CHANNEL(src_image,dst_image,red);
            break;
          }
        case GreenChannel:
        case MagentaChannel:
          {
            EXPORT_IMAGE_CHANNEL(src_image,dst_image,green);
            break;
          }
        case BlueChannel:
        case YellowChannel:
          {
            EXPORT_IMAGE_CHANNEL(src_image,dst_image,blue);
            break;
          }
        case OpacityChannel:
          {
            if (src_image->colorspace == CMYKColorspace)
              {
                register const IndexPacket
                  *src_indexes;
                
                src_indexes=GetIndexes(src_image);
                for (x=(long) dst_image->columns; x > 0; x--)
                  {
                    q->red=q->green=q->blue=*src_indexes++;
                    q->opacity=OpaqueOpacity;
                    q++;
                  }
              }
            else
              {
                EXPORT_IMAGE_CHANNEL(src_image,dst_image,opacity);
              }
            break;
          }
        case MatteChannel:
        case BlackChannel:
          {
            EXPORT_IMAGE_CHANNEL(src_image,dst_image,opacity);
            break;
          }
        default:
          {
          }
        }

      if (!SyncImagePixels(dst_image))
        break;
    }
  dst_image->is_grayscale=True;
  dst_image->is_monochrome=src_image->is_monochrome;
  return dst_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C h a n n e l D e p t h                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageChannelDepth() returns the minimum bit depth required to store
%  the specified image channel without actual loss of color resolution.
%  Pixel components are stored in a Quantum, which is 8, 16, or 32 bits
%  depending on the QuantumDepth value set when the software is compiled.
%  GetImageChannelDepth() returns the smallest modulus storage size which
%  supports the scale of the pixel within the range (i.e. no information is
%  lost). As an example, the value one is returned for a bilevel channel
%  since only one bit of resolution is required to represent a bilevel channel.
%
%  The format of the GetImageChannelDepth method is:
%
%      unsigned long GetImageChannelDepth(const Image *image,
%                      const ChannelType channel,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to test.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define COMPUTE_CHANNEL_DEPTH(depth,parameter) \
{ \
  long \
    y; \
\
  depth=1; \
  for (y=0; y < (long) image->rows; y++) \
    { \
      register const PixelPacket \
        *p; \
\
      register long \
        x; \
\
      register IndexPacket \
        *indexes; \
\
      register unsigned int \
        scale; \
\
      p=AcquireImagePixels(image,0,y,image->columns,1,exception); \
      if (p == (const PixelPacket *) NULL) \
        break; \
      indexes=GetIndexes(image); \
      scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth)); \
      x=(long) image->columns; \
      while(x > 0) \
        { \
          if ((parameter) != scale*((parameter)/scale)) \
            { \
              depth++; \
              if (depth == QuantumDepth) \
                break; \
              scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth)); \
              continue; \
            } \
          p++; \
          indexes++; \
          x--; \
        } \
      if (depth == QuantumDepth) \
        break; \
    } \
}

MagickExport unsigned int GetImageChannelDepth(const Image *image,
  const ChannelType channel, ExceptionInfo *exception)
{
  unsigned int
    depth=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->red);
        break;
      }
    case GreenChannel:
    case MagentaChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->green);
        break;
      }
    case BlueChannel:
    case YellowChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->blue);
        break;
      }
    case OpacityChannel:
      {
        if (image->colorspace == CMYKColorspace)
          {
            COMPUTE_CHANNEL_DEPTH(depth,*indexes);
          }
        else
          {
            COMPUTE_CHANNEL_DEPTH(depth,p->opacity);
          }
        break;
      }
    case BlackChannel:
    case MatteChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->opacity);
        break;
      }
    default:
      {
      }
    }

  return depth;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     I m p o r t I m a g e C h a n n e l                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportImageChannel() imports an image into the specified image channel.
%
%  The format of the ImportImageChannel method is:
%
%      MagickPassFail ImportImageChannel(const Image *src_image,
%                                        Image *dst_image,
%                                        const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o src_image: The image to use as the replacement image channel.
%
%    o dst_image: The image to import the channel into.
%
%    o channel: The image channel to import
%
%
*/
#define IMPORT_IMAGE_CHANNEL(src_image,dst_image,target) \
do { \
  register IndexPacket \
    *dst_indexes; \
\
  dst_indexes=GetIndexes(dst_image); \
  if (src_image->storage_class == PseudoClass) \
    { \
      register const IndexPacket \
        *src_indexes; \
\
      src_indexes=GetIndexes(src_image); \
      if (src_image->is_grayscale) \
        for (x=(long) dst_image->columns; x > 0; x--) \
          target=src_image->colormap[*src_indexes++].red; \
      else \
        for (x=(long) dst_image->columns; x > 0; x--) \
          { \
            target=PixelIntensityToQuantum(&src_image->colormap[*src_indexes]); \
            src_indexes++; \
          } \
    } \
  else \
    { \
      if (src_image->is_grayscale) \
        for (x=(long) dst_image->columns; x > 0; x--) \
          target=p++->red; \
      else \
        for (x=(long) dst_image->columns; x > 0; x--) \
          { \
            target=PixelIntensityToQuantum(p); \
            p++; \
          } \
    } \
} while (0);

MagickPassFail ImportImageChannel(const Image *src_image,
                                  Image *dst_image,
                                  const ChannelType channel)
{
#define ImageImageChannelText  "  Import a channel into the image...  "
  long
    y;

  MagickPassFail
    status=MagickPass;

  assert(dst_image != (Image *) NULL);
  assert(dst_image->signature == MagickSignature);
  assert(src_image != (Image *) NULL);
  assert(src_image->signature == MagickSignature);

  /*
    Ensure that destination image is DirectClass
  */
  if (dst_image->storage_class == PseudoClass)
    {
      status &= SyncImage(dst_image);
      dst_image->storage_class=DirectClass;
    }

  for (y=0; y < (long) dst_image->rows; y++)
    {
      register long
        x;
  
      register const PixelPacket
        *p;

      register PixelPacket
        *q;
  
      q=GetImagePixels(dst_image,0,y,dst_image->columns,1);
      if (q == (PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }
      p=AcquireImagePixels(src_image,0,y,src_image->columns,1,&dst_image->exception);
      if (p == (PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }

      switch (channel)
        {
        case RedChannel:
        case CyanChannel:
          {
            IMPORT_IMAGE_CHANNEL(src_image,dst_image,q++->red);
            break;
          }
        case GreenChannel:
        case MagentaChannel:
          {
            IMPORT_IMAGE_CHANNEL(src_image,dst_image,q++->green);
            break;
          }
        case BlueChannel:
        case YellowChannel:
          {
            IMPORT_IMAGE_CHANNEL(src_image,dst_image,q++->blue);
            break;
          }
        case OpacityChannel:
          {
            if (dst_image->colorspace == CMYKColorspace)
              {
                IMPORT_IMAGE_CHANNEL(src_image,dst_image,*dst_indexes++);
              }
            else
              {
                IMPORT_IMAGE_CHANNEL(src_image,dst_image,q++->opacity);
              }
            break;
          }
        case MatteChannel:
        case BlackChannel:
          {
            IMPORT_IMAGE_CHANNEL(src_image,dst_image,q++->opacity);
            break;
          }
        default:
          {
          }
        }
      if (!SyncImagePixels(dst_image))
        {
          status=MagickFail;
          break;
        }
      if (QuantumTick(y,dst_image->rows))
        if (!MagickMonitor(ImageImageChannelText,y,dst_image->rows,&dst_image->exception))
          break;
    }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e C h a n n e l D e p t h                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageChannelDepth() translates the pixel quantums in the specified
%  channel so that if they are later divided to fit within the specified bit
%  depth, that no additional information is lost (i.e. no remainder resulting
%  from the division). Note that any subsequent image processing is likely
%  to increase the effective depth of the image channels. A non-zero
%  value is returned if the operation is successful. Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the SetImageChannelDepth method is:
%
%      MagickPassFail SetImageChannelDepth(Image *image,
%                                          const ChannelType channel,
%                                          const unsigned int depth)
%
%  A description of each parameter follows:
%
%    o image: The image to update.
%
%    o channel: Channel to modify.
%
%    o depth: Desired channel depth (range 1 to QuantumDepth)
%
%
*/
#define SET_CHANNEL_DEPTH(image,desired_depth,channel,parameter,status) \
{ \
  long \
    y; \
\
  register long \
    x; \
\
  register unsigned long \
    scale; \
\
  register PixelPacket \
    *q; \
\
  status=MagickPass; \
\
  scale=MaxRGB / (MaxRGB >> (QuantumDepth-desired_depth)); \
  for (y=0; y < (long) image->rows; y++) \
    { \
      q=GetImagePixels(image,0,y,image->columns,1); \
      if (q == (PixelPacket *) NULL) \
        { \
          status=MagickFail; \
          break; \
        } \
      for (x=0; x < (long) image->columns; x++) \
        { \
          parameter=scale*((parameter)/scale); \
          q++; \
        } \
      if (!SyncImagePixels(image)) \
        { \
          status=MagickFail; \
          break; \
        } \
    } \
  if (image->storage_class == PseudoClass) \
    { \
      register long \
        i; \
\
      q=image->colormap; \
      for (i=0; i < (long) image->colors; i++) \
        { \
          parameter=scale*((parameter)/scale); \
          q++; \
        } \
    } \
}

MagickExport MagickPassFail SetImageChannelDepth(Image *image,
  const ChannelType channel, const unsigned int depth)
{
  unsigned long
    current_depth,
    desired_depth;

  unsigned int
    is_grayscale,
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  status=MagickPass;
  is_grayscale=image->is_grayscale;

  desired_depth=depth;
  if (desired_depth > QuantumDepth)
    desired_depth=QuantumDepth;

  current_depth=GetImageChannelDepth(image,channel,&image->exception);

  if (current_depth > desired_depth)
    {
      switch (channel)
        {
        case RedChannel:
        case CyanChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->red,status);
            break;
          }
        case GreenChannel:
        case MagentaChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->green,status);
            break;
          }
        case BlueChannel:
        case YellowChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->blue,status);
            break;
          }
        case OpacityChannel:
          {
            if (image->colorspace == CMYKColorspace)
              {
                long
                  y;
                
                register IndexPacket
                  *indexes;
                
                register long
                  x;
                
                register PixelPacket
                  *q;

                register unsigned long
                  scale;

                scale=MaxRGB / (MaxRGB >> (QuantumDepth-desired_depth));
                for (y=0; y < (long) image->rows; y++)
                  {
                    q=GetImagePixels(image,0,y,image->columns,1);
                    if (q == (PixelPacket *) NULL)
                      break;
                    indexes=GetIndexes(image);
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        *indexes=scale*((*indexes)/scale);
                        indexes++;
                      }
                    if (!SyncImagePixels(image))
                      break;
                  }
              }
            else
              {
                SET_CHANNEL_DEPTH(image,desired_depth,channel,q->opacity,status);
              }
            break;
          }
        case BlackChannel:
        case MatteChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->opacity,status);
            break;
          }
        default:
          {
          }
        }
    }

  image->is_grayscale=is_grayscale;

  return status;
}
