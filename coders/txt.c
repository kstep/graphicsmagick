/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            TTTTT  X   X  TTTTT                              %
%                              T     X X     T                                %
%                              T      X      T                                %
%                              T     X X     T                                %
%                              T    X   X    T                                %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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

/*
  Forward declarations.
*/
static unsigned int
  WriteTXTImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T X T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTXTImage reads a text file and returns it as an image.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTXTImage method is:
%
%      Image *ReadTXTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTXTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTXTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    text[MaxTextExtent];

  double
    dx_resolution,
    dy_resolution;

  DrawInfo
    *draw_info;

  Image
    *image,
    *texture;

  int
    count,
    offset;

  RectangleInfo
    page;

  register char
    *p;

  unsigned int
    height,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      char
        density[MaxTextExtent];

      (void) strcpy(density,PSDensityGeometry);
      count=sscanf(density,"%lfx%lf",&image->x_resolution,&image->y_resolution);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  page.width=612;
  page.height=792;
  page.x=0;
  page.y=0;
  (void) ParseImageGeometry("612x792+43+43",&page.x,&page.y,
    &page.width,&page.height);
  if (image_info->page != (char *) NULL)
    (void) ParseImageGeometry(image_info->page,&page.x,&page.y,
      &page.width,&page.height);
  /*
    Initialize Image structure.
  */
  image->columns=(unsigned int)
    ceil(((page.width*image->x_resolution)/dx_resolution)-0.5);
  image->rows=(unsigned int)
    ceil(((page.height*image->y_resolution)/dy_resolution)-0.5);
  (void) QueryColorDatabase("#c0c0c0",&image->background_color);
  texture=(Image *) NULL;
  if (image_info->texture != (char *) NULL)
    {
      ImageInfo
        *clone_info;

      clone_info=CloneImageInfo(image_info);
      if (clone_info == (ImageInfo *) NULL)
        return((Image *) NULL);
      (void) strcpy(clone_info->filename,image_info->texture);
      texture=ReadImage(clone_info,exception);
      if (texture != (Image *) NULL)
        TextureImage(image,texture);
      DestroyImageInfo(clone_info);
    }
  /*
    Annotate the text image.
  */
  SetImage(image,OpaqueOpacity);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  (void) strcpy(filename,image_info->filename);
  offset=0;
  for ( ; ; )
  {
    /*
      Annotate image with text.
    */
    p=ReadBlobString(image,text);
    if (p == (char *) NULL)
      break;
    (void) CloneString(&draw_info->text,text);
    FormatString(geometry,"%+d%+d",page.x,page.y+offset);
    (void) CloneString(&draw_info->geometry,geometry);
    AnnotateImage(image,draw_info);
    height=(unsigned int) (draw_info->pointsize*
      AbsoluteValue(Max(draw_info->affine.sx,draw_info->affine.rx)));
    offset+=height;
    if (image->previous == (Image *) NULL)
      if (QuantumTick(page.y+offset,image->rows))
        MagickMonitor(LoadImageText,page.y+offset,image->rows);
    if (((2*page.y)+offset+height) < image->rows)
      continue;
    /*
      Page is full-- allocate next image structure.
    */
    image->next=CloneImage(image,image->columns,image->rows,True,exception);
    if (image->next == (Image *) NULL)
      {
        DestroyDrawInfo(draw_info);
        return((Image *) NULL);
      }
    (void) strcpy(image->next->filename,filename);
    image->next->blob=image->blob;
    image->next->file=image->file;
    image->next->filesize=image->blob.filesize;
    image->next->scene=image->scene+1;
    image->next->previous=image;
    image=image->next;
    SetImage(image,OpaqueOpacity);
    MagickMonitor(LoadImagesText,TellBlob(image),image->blob.filesize);
    /*
      Initialize text image to background color.
    */
    if (texture != (Image *) NULL)
      {
        MonitorHandler
          handler;

        handler=SetMonitorHandler((MonitorHandler) NULL);
        TextureImage(image,texture);
        (void) SetMonitorHandler(handler);
      }
    offset=0;
  }
  if (texture != (Image *) NULL)
    DestroyImage(texture);
  DestroyDrawInfo(draw_info);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T X T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTXTImage adds attributes for the TXT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTXTImage method is:
%
%      RegisterTXTImage(void)
%
*/
ModuleExport void RegisterTXTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("TEXT");
  entry->decoder=ReadTXTImage;
  entry->encoder=WriteTXTImage;
  entry->raw=True;
  entry->description=AllocateString("Raw text");
  entry->module=AllocateString("TXT");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("TXT");
  entry->decoder=ReadTXTImage;
  entry->encoder=WriteTXTImage;
  entry->description=AllocateString("Raw text");
  entry->module=AllocateString("TXT");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T X T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTXTImage removes format registrations made by the
%  TXT module from the list of supported formats.
%
%  The format of the UnregisterTXTImage method is:
%
%      UnregisterTXTImage(void)
%
*/
ModuleExport void UnregisterTXTImage(void)
{
  UnregisterMagickInfo("TEXT");
  UnregisterMagickInfo("TXT");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T X T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTXTImage writes the pixel values as text numbers.
%
%  The format of the WriteTXTImage method is:
%
%      unsigned int WriteTXTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteTXTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteTXTImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  int
    y;

  register int
    x;

  register PixelPacket
    *p;

  unsigned int
    scene,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Convert MIFF to TXT raster pixels.
    */
    TransformRGBImage(image,RGBColorspace);
    for (y=0; y < (int) image->rows; y++)
    {
      p=GetImagePixels(image,0,y,image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) image->columns; x++)
      {
        if (image->matte)
          {
            FormatString(buffer,"%d,%d: %u,%u,%u,%u\n",x,y,
              p->red,p->green,p->blue,p->opacity);
            (void) WriteBlobString(image,buffer);
          }
        else
          {
            FormatString(buffer,"%d,%d: %u,%u,%u  ",x,y,
              p->red,p->green,p->blue);
            (void) WriteBlobString(image,buffer);
	    (void) QueryColorname(image,p,AllCompliance,buffer);
            (void) WriteBlobString(image,buffer);
          }
        (void) WriteBlobByte(image,'\n');
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            MagickMonitor(SaveImageText,y,image->rows);
        p++;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
